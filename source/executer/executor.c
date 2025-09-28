/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/21 16:22:21 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

/* ************************************************************************** */
/*                            MAIN EXECUTOR FUNCTION                         */
/* ************************************************************************** */

int	execute_commands(t_command *commands, t_global *global)
{
	if (!commands)
		return (0);
	if (commands->next)
		return (execute_pipeline(commands, global)); // BURADAYIZ
	else
		return (execute_single_command(commands, global));
}

/* ************************************************************************** */
/*                            SINGLE COMMAND EXECUTION                       */
/* ************************************************************************** */

int	execute_single_command(t_command *cmd, t_global *global)
{

	if ((!cmd || !cmd->args || !cmd->args[0]) && cmd->redirections)
	{
		return (execute_redirect_command(cmd, global));
	}
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);

	if (is_builtin(cmd->args[0]))
		return (execute_builtin(cmd, global));
	return (execute_external_command(cmd, global));
}

/* ************************************************************************** */
/*                            EXTERNAL COMMAND EXECUTION                     */
/* ************************************************************************** */

int	execute_redirect_command(t_command *cmd, t_global *global) // Burada sıkıntı var leek çıkartıyor '<<end' inputunda ve else eklenecek bi altta ki fonksiyon tarzı
{
	pid_t	pid;
	int	status;

	pid = 0;
	status = 0;

	pid = fork();
	if (pid == 0)
	{
		setup_child_signals();
		global->in_child = 1;
		setup_redirections(cmd);
		clear_garbage();
		exit(0);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0); // Burada status pid numaralı process'in döndürdüğü exit status
		
		// Signal ile sonlandı mı kontrol et
		if (WIFSIGNALED(status))
		{
			int signal_num = WTERMSIG(status);
			if (signal_num == SIGINT)
				return (130); // 128 + SIGINT
			else if (signal_num == SIGQUIT)
				return (131); // 128 + SIGQUIT
			else
				return (128 + signal_num);
		}
		//clear_garbage();
		return (WEXITSTATUS(status));
	}
	return (1);
}

int	execute_external_command(t_command *cmd, t_global *global)
{
	pid_t	pid;
	int		status;
	char	*path;

	path = find_command_path(cmd->args[0], global->env_list); // Path içerisinde command arıyor bulduğu path'in yanına "/cmd->args[0]" ekliyor

	if (!path) // Path içerisinde command bulunamazsa hata
	{
		printf("minishell: %s: command not found\n", cmd->args[0]);
		return (127);
	}
	pid = fork(); // child process açıyor ve child process kendi pid numarasını 0 olarak biliyor
	if (pid == 0)
	{
		// Child process - sinyalleri default davranışa çevir

		setup_child_signals();

		global->in_child = 1;
		setup_redirections(cmd); // Öğrenilecek (dup ve dup2)
		execve(path, cmd->args, env_list_to_array(global->env_list)); // Öğrenilecek!
		perror("execve");
		clear_garbage();
		exit(127);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0); // Burada status pid numaralı process'in döndürdüğü exit status
		// free(path);
		
		// Signal ile sonlandı mı kontrol et
		if (WIFSIGNALED(status))
		{
			int signal_num = WTERMSIG(status);
			if (signal_num == SIGINT)
				return (130); // 128 + SIGINT
			else if (signal_num == SIGQUIT)
				return (131); // 128 + SIGQUIT
			else
				return (128 + signal_num);
		}
		//clear_garbage();
		return (WEXITSTATUS(status));
	}
	else
	{
		perror("fork");
		free(path);
		clear_garbage();
		return (1);
	}
}

/* ************************************************************************** */
/*                            PIPELINE EXECUTION                             */
/* ************************************************************************** */

static int	preprocess_heredocs(t_command *commands, t_global *global)
{
	t_command	*current;
	t_list		*redirect_node;
	t_redirect	*redirect;

	(void)global;
	current = commands;
	while (current)
	{
		if (current->redirections)
		{
			redirect_node = current->redirections;
			while (redirect_node)
			{
				redirect = (t_redirect *)redirect_node->content;
				if (redirect && redirect->type == T_HEREDOC)
				{
					printf("BURASI İLK preprocess_heredocs");
					// Heredoc'u main process'te işle
					redirect->fd = handle_heredoc(redirect);
					if (redirect->fd == -1)
						return (-1);
				}
				redirect_node = redirect_node->next;
			}
		}
		current = current->next;
	}
	// close(redirect->fd);
	return (0);
}

int	execute_pipeline(t_command *commands, t_global *global)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_fd;
	int			last_status;
	pid_t		*pids;
	int			cmd_count;
	int			i;

	// Pre-process heredocs in main process BEFORE forking
	if (preprocess_heredocs(commands, global) == -1)
		return (1);

	// Count commands
	current = commands;
	cmd_count = 0;
	while (current)
	{
		cmd_count++;
		current = current->next;
	}
	
	pids = halloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return (1);
	
	current = commands;
	prev_fd = 0;
	i = 0;
	last_status = 0;
	
	while (current && i < cmd_count)
	{
		if (current->next && pipe(pipe_fd) == -1)
		{
			perror("pipe");
			return (1);
		}
		
		pids[i] = execute_pipeline_command_async(current, global, prev_fd, pipe_fd);
		if (pids[i] == -1)
			return (1);
		
		// Pipeline'da normal flow - redirection bekleme yapmıyoruz
		// Race condition çözümü için farklı strateji gerekiyor
		
		if (prev_fd != 0) // BURADA AÇIK FD OLABİLİR!!!!!!
			close(prev_fd);
		if (current->next)
		{
			close(pipe_fd[1]);
			prev_fd = pipe_fd[0];
		}
		current = current->next;
		i++;
	}
	// Wait for remaining children (42 norm - while)
	i = 0;
	while (i < cmd_count)
	{
		int status;
		if (pids[i] > 0) // Henüz beklenmemiş
		{
			waitpid(pids[i], &status, 0);
			if (i == cmd_count - 1)  // Last command's exit status
			{
				if (WIFSIGNALED(status))
				{
					int signal_num = WTERMSIG(status);
					if (signal_num == SIGINT)
						last_status = 130;
					else if (signal_num == SIGQUIT)
						last_status = 131;
					else
						last_status = 128 + signal_num;
				}
				else
					last_status = WEXITSTATUS(status);
			}
		}
		i++;
	}

	current = commands;
	while (current)
	{
		if (current->redirections)
		{
			t_list *node = current->redirections;
			while (node)
			{
				t_redirect *redirect = (t_redirect *)node->content;
				if (redirect && redirect->type == T_HEREDOC && redirect->fd > 0)
				{
					printf("HELLOOO\n");
					close(redirect->fd);
					redirect->fd = -1;
				}
			node = node->next;
			}
		}
		current = current->next;
	}
	
	return (last_status);
}

/* ************************************************************************** */
/*                            ASYNC PIPELINE COMMAND EXECUTION              */
/* ************************************************************************** */

pid_t	execute_pipeline_command_async(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
	pid_t	pid;
	char	*path;
	int		exit_num;

	// Pipeline'da tüm komutlar (built-in dahil) child process'te çalışmalı
	pid = fork();
	exit_num = 0;
	if (pid == 0)
	{
		// Child process - sinyalleri default davranışa çevir
		setup_child_signals();
		global->in_child = 1;
		

		printf("ARGS $%s$\n", cmd->args[0]);
		setup_pipeline_fds(cmd, prev_fd, pipe_fd);
		printf("ARGS $%s$\n", cmd->args[0]);

		setup_redirections(cmd);


		// AÇIK FD BURADA DEĞİL
		
		// printf("prev_fd: %d - pipe_Fd[0]: %d - pipe_fd[1]: %d \n", prev_fd, pipe_fd[0], pipe_fd[1]);

		// if (prev_fd > 0)
		// {
		// 	close(prev_fd);
		// 	prev_fd = -1;
		// }
		// if (pipe_fd[0] > 0)
		// {
		// 	close(pipe_fd[0]);
		// 	pipe_fd[0] = -1;
		// }
		// if (pipe_fd[1] > 0)
		// {
		// 	close(pipe_fd[1]);
		// 	pipe_fd[1] = -1;
		// }
		// printf("prev_fd: %d - pipe_Fd[0]: %d - pipe_fd[1]: %d \n", prev_fd, pipe_fd[0], pipe_fd[1]);
		
		// printf("ARGS[0]: $%s$\n", cmd->args[0]);


		if (is_builtin(cmd->args[0]))
		{
			execute_builtin(cmd, global);
			exit_num = global->exit_status;
			clear_garbage();
			exit(exit_num);
		}
		

		path = find_command_path(cmd->args[0], global->env_list);
		if (!path)
		{
			printf("minishell: %s: command not found\n", cmd->args[0]);
			exit(127);
		}
		
		execve(path, cmd->args, env_list_to_array(global->env_list));
		perror("execve");
		exit(127);
	}
	else if (pid > 0)
	{
		return (pid);
	}
	else
	{
		perror("fork");
		return (-1);
	}
}


