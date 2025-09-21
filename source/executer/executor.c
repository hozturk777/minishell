/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/20 18:58:33 by hasivaci         ###   ########.fr       */
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
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (is_builtin(cmd->args[0]))
		return (execute_builtin(cmd, global));
	return (execute_external_command(cmd, global));
}

/* ************************************************************************** */
/*                            EXTERNAL COMMAND EXECUTION                     */
/* ************************************************************************** */

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

int	execute_pipeline(t_command *commands, t_global *global)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_fd;
	int			last_status;
	pid_t		*pids;
	int			cmd_count;
	int			i;

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
	
	while (current)
	{
		if (current->next && pipe(pipe_fd) == -1)
		{
			perror("pipe");
			// free(pids);
			return (1);
		}
		
		pids[i] = execute_pipeline_command_async(current, global, prev_fd, pipe_fd);
		if (pids[i] == -1)
		{
			// free(pids);
			return (1);
		}
		
		if (prev_fd != 0)
			close(prev_fd);
		if (current->next)
		{
			close(pipe_fd[1]);
			prev_fd = pipe_fd[0];
		}
		current = current->next;
		i++;
	}
	
	// Wait for all children
	for (i = 0; i < cmd_count; i++)
	{
		int status;
		if (pids[i] > 0)
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
	}
	
	// free(pids);
	return (last_status);
}

/* ************************************************************************** */
/*                            ASYNC PIPELINE COMMAND EXECUTION              */
/* ************************************************************************** */

pid_t	execute_pipeline_command_async(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
	pid_t	pid;
	char	*path;

	// Pipeline'da tüm komutlar (built-in dahil) child process'te çalışmalı
	pid = fork();
	if (pid == 0)
	{
		// Child process - sinyalleri default davranışa çevir
		setup_child_signals();
		global->in_child = 1;
		
		setup_pipeline_fds(cmd, prev_fd, pipe_fd);
		setup_redirections(cmd);
		
		if (is_builtin(cmd->args[0]))
		{
			execute_builtin(cmd, global);
			exit(global->exit_status);
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

/* ************************************************************************** */

int	execute_pipeline_command(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
	pid_t	pid;
	int		status;
	char	*path;

	if (is_builtin(cmd->args[0]) && !cmd->next && prev_fd == 0)
		return (execute_builtin(cmd, global));
	path = find_command_path(cmd->args[0], global->env_list);
	if (!path)
		return (127);
	pid = fork();
	if (pid == 0)
	{
		// Child process - sinyalleri default davranışa çevir
		setup_child_signals();
		global->in_child = 1;
		
		setup_pipeline_fds(cmd, prev_fd, pipe_fd);
		setup_redirections(cmd);
		if (is_builtin(cmd->args[0]))
		{
			execute_builtin(cmd, global);
			exit(global->exit_status);
		}
		execve(path, cmd->args, env_list_to_array(global->env_list));
		perror("execve");
		exit(127);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		free(path);
		
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
		
		return (WEXITSTATUS(status));
	}
	else
	{
		perror("fork");
		free(path);
		return (1);
	}
}
