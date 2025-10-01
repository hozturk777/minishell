/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abakirca <abakirca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/10/01 18:45:21 by abakirca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// /* ************************************************************************** */
// /*                            MAIN EXECUTOR FUNCTION                         */
// /* ************************************************************************** */

// int	execute_commands(t_command *commands, t_global *global)
// {
// 	if (!commands)
// 		return (0);
// 	if (commands->next)
// 		return (execute_pipeline(commands, global)); // BURADAYIZ
// 	else
// 		return (execute_single_command(commands, global));
// }

// /* ************************************************************************** */
// /*                            SINGLE COMMAND EXECUTION                       */
// /* ************************************************************************** */

// int	execute_single_command(t_command *cmd, t_global *global)
// {

// 	if ((!cmd || !cmd->args || !cmd->args[0]) && cmd->redirections)
// 	{
// 		return (execute_redirect_command(cmd, global));
// 	}
// 	if (!cmd || !cmd->args || !cmd->args[0])
// 		return (1);

// 	if (is_builtin(cmd->args[0]))
// 		return (execute_builtin(cmd, global));
// 	return (execute_external_command(cmd, global));
// }

// /* ************************************************************************** */
// /*                            EXTERNAL COMMAND EXECUTION                     */
// /* ************************************************************************** */


// static void	handle_redirect_child_process(t_command *cmd, t_global *global)
// {
//     /* Setup child signal handlers */
//     setup_child_signals();
//     global->in_child = 1;
//     /* Setup redirections for the command */
//     setup_redirections(cmd);
//     /* Exit after setup - redirect only command */
//     cleanup_and_exit();
//     exit(0);
// }

// static int	wait_for_redirect_process(pid_t pid)
// {
// 	int	status;

//     /* Wait for redirect process to complete */
//     waitpid(pid, &status, 0);
//     /* Handle signal termination */
//     if (WIFSIGNALED(status))
//     {
// 		int signal_num = WTERMSIG(status);
//         /* Return appropriate exit code for signals */
//         if (signal_num == SIGINT)
// 		return (130);
//         else if (signal_num == SIGQUIT)
// 		return (131);
//         else
// 		return (128 + signal_num);
//     }
//     /* Return normal exit status */
//     return (WEXITSTATUS(status));
// }

// int	execute_redirect_command(t_command *cmd, t_global *global)
// {
// 	pid_t	pid;

// 	/* Fork child process for redirect command */
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		/* Child process handles redirect execution */
// 		handle_redirect_child_process(cmd, global);
// 	}
// 	else if (pid > 0)
// 	{
// 		/* Parent process waits for child completion */
// 		return (wait_for_redirect_process(pid));
// 	}
// 	/* Fork failed */
// 	return (1);
// }

/* ************************************************************************** */
/*                            PIPELINE EXECUTION                             */
/* ************************************************************************** */

int	preprocess_heredocs(t_command *commands, t_global *global)
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

/* ************************************************************************** */
/*                            HEREDOC FD CLEANUP                             */
/* ************************************************************************** */

static void	close_unused_heredoc_fds(t_command *cmd)
{
	t_list		*node;
	t_redirect	*redirect;

	if (!cmd->redirections)
		return ;
	
	node = cmd->redirections;
	while (node)
	{
		redirect = (t_redirect *)node->content;
		if (redirect && redirect->type == T_HEREDOC && redirect->fd > 2)
		{
			// dup2 yapıldıktan sonra orijinal FD'yi kapat
			close(redirect->fd);
			redirect->fd = -1;
		}
		node = node->next;
	}
}

/* ************************************************************************** */
/*                            ASYNC PIPELINE COMMAND EXECUTION              */
/* ************************************************************************** */
// YORUM satırları kaldırıınca 52 satır kod bloğu;
pid_t	execute_pipeline_command_async(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
	pid_t		pid;
	char		*path;
	int			exit_num;
	t_redirect	*redirect;
	t_list		*node;
	int			originals[2];

	// Pipeline'da tüm komutlar (built-in dahil) child process'te çalışmalı
	node = cmd->redirections; // burası açıkken 'echo hello >> test.txt | cat test.txt' çalışmıyor kapalıyken de 'echo hello | <<end' command not found veriyor!!
	pid = fork();
	exit_num = 0;
	if (pid == 0)
	{
		// Child process - sinyalleri default davranışa çevir
		setup_child_signals();
		global->in_child = 1;
		// printf("ARGS $%s$\n", cmd->args[0]);
		originals[0] = dup(STDIN_FILENO);
		originals[1] = dup(STDOUT_FILENO);
		setup_pipeline_fds(cmd, prev_fd, pipe_fd);
		// printf("ARGS $%s$\n", cmd->args[0]);

		setup_redirections(cmd);
		
		// Child'da kullanılmayan heredoc FD'leri kapat
		close_unused_heredoc_fds(cmd);
		close_all_heredoc_fds();

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
			execute_builtin(cmd, global, originals);
			exit_num = global->exit_status;
			cleanup_and_exit();
			exit(exit_num);
		}
		

		path = find_command_path(cmd->args[0], global->env_list);

		// t_redirect *redirect;
		// redirect = (t_redirect *)cmd->redirections->content;
		// if (!path)
		// {
		// 	if (redirect->type == T_HEREDOC)
		// 	{
		// 		cleanup_and_exit();
		// 		exit(0);
		// 	}
			
		// 	printf("minishell: %s: command not found\n", cmd->args[0]);
		// 	cleanup_and_exit();
		// 	exit(127);
		// }
		
        if (!path)
        {
			while (node)
			{
				redirect = (t_redirect *)node->content;
				if (redirect->type == T_HEREDOC)
				{
					close(originals[0]);
					close(originals[1]);
					cleanup_and_exit();
					exit(0);
				}
				node = node->next;
			}
			
            printf("minishell: %s: command not found\n", cmd->args[0]);
			close(originals[0]);
			close(originals[1]);
            cleanup_and_exit();
            exit(127);
        }
		execve(path, cmd->args, env_list_to_array(global->env_list));
		perror("execve");
		cleanup_and_exit();
		close(originals[0]);
		close(originals[1]);
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


// int	execute_redirect_command(t_command *cmd, t_global *global) // Burada sıkıntı var leek çıkartıyor '<<end' inputunda ve else eklenecek bi altta ki fonksiyon tarzı
// {
	// 	pid_t	pid;
	// 	int	status;
	
	// 	pid = 0;
	// 	status = 0;
	
	// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		setup_child_signals();
// 		global->in_child = 1;
// 		setup_redirections(cmd);
// 		cleanup_and_exit();
// 		exit(0);
// 	}
// 	else if (pid > 0)
// 	{
// 		waitpid(pid, &status, 0); // Burada status pid numaralı process'in döndürdüğü exit status
		
// 		// Signal ile sonlandı mı kontrol et
// 		if (WIFSIGNALED(status))
// 		{
// 			int signal_num = WTERMSIG(status);
// 			if (signal_num == SIGINT)
// 				return (130); // 128 + SIGINT
// 			else if (signal_num == SIGQUIT)
// 				return (131); // 128 + SIGQUIT
// 			else
// 				return (128 + signal_num);
// 		}
// 		//clear_garbage();
// 		return (WEXITSTATUS(status));
// 	}
// 	return (1);
// }

// // int	execute_external_command(t_command *cmd, t_global *global)
// // {
// // 	pid_t	pid;
// // 	int		status;
// // 	char	*path;

// // 	path = find_command_path(cmd->args[0], global->env_list); // Path içerisinde command arıyor bulduğu path'in yanına "/cmd->args[0]" ekliyor

// // 	if (!path) // Path içerisinde command bulunamazsa hata
// // 	{
// // 		printf("minishell: %s: command not found\n", cmd->args[0]);
// // 		return (127);
// // 	}
// // 	pid = fork(); // child process açıyor ve child process kendi pid numarasını 0 olarak biliyor
// // 	if (pid == 0)
// // 	{
// // 		// Child process - sinyalleri default davranışa çevir

// // 		setup_child_signals();

// // 		global->in_child = 1;
// // 		setup_redirections(cmd); // Öğrenilecek (dup ve dup2)
// // 		execve(path, cmd->args, env_list_to_array(global->env_list)); // Öğrenilecek!
// // 		perror("execve");
// // 		cleanup_and_exit();
// // 		exit(127);
// // 	}
// // 	else if (pid > 0)
// // 	{
// // 		waitpid(pid, &status, 0); // Burada status pid numaralı process'in döndürdüğü exit status
// // 		// free(path);
		
// // 		// Signal ile sonlandı mı kontrol et
// // 		if (WIFSIGNALED(status))
// // 		{
// // 			int signal_num = WTERMSIG(status);
// // 			if (signal_num == SIGINT)
// // 				return (130); // 128 + SIGINT
// // 			else if (signal_num == SIGQUIT)
// // 				return (131); // 128 + SIGQUIT
// // 			else
// // 				return (128 + signal_num);
// // 		}
// // 		//clear_garbage();
// // 		return (WEXITSTATUS(status));
// // 	}
// // 	else
// // 	{
// // 		perror("fork");
// // 		free(path);
// // 		clear_garbage();
// // 		return (1);
// // 	}
// // }


// static void	handle_external_child_process(t_command *cmd, t_global *global, char *path)
// {
// 	/* Setup child signal handlers */
//     setup_child_signals();
//     global->in_child = 1;
//     /* Setup redirections before execution */
//     setup_redirections(cmd);
//     /* Execute the external command */
//     execve(path, cmd->args, env_list_to_array(global->env_list));
//     /* execve failed - cleanup and exit */
//     perror("execve");
//     cleanup_and_exit();
//     exit(127);
// }

// static int	wait_for_external_process(pid_t pid, char *path)
// {
// 	int	status;

//     /* Free path before waiting */
//     free(path);
//     clear_garbage();
//     /* Wait for external process to complete */
//     waitpid(pid, &status, 0);
//     /* Handle signal termination */
//     if (WIFSIGNALED(status))
//     {
//         int signal_num = WTERMSIG(status);
//         /* Return appropriate exit code for signals */
//         if (signal_num == SIGINT)
// 		return (130);
//         else if (signal_num == SIGQUIT)
// 		return (131);
//         else
// 		return (128 + signal_num);
//     }
//     /* Return normal exit status */
//     return (WEXITSTATUS(status));
// }
// int	execute_external_command(t_command *cmd, t_global *global)
// {
// 	char	*path;
// 	pid_t	pid;

// 	/* Find the command path in system PATH */
// 	path = find_command_path(cmd->args[0], global->env_list);
// 	if (!path)
// 	{
// 		/* Command not found in PATH */
// 		printf("minishell: %s: command not found\n", cmd->args[0]);
// 		return (127);
// 	}
// 	/* Fork process for external command execution */
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		/* Child process executes the external command */
// 		handle_external_child_process(cmd, global, path);
// 	}
// 	else if (pid > 0)
// 	{
// 		/* Parent process waits for child completion */
// 		return (wait_for_external_process(pid, path));
// 	}
// 	else
// 	{
// 		/* Fork failed - cleanup and return error */
// 		perror("fork");
// 		free(path);
// 		clear_garbage();
// 		return (1);
// 	}
// }

/* ************************************************************************** */
/*                            PIPELINE EXECUTION                             */
/* ************************************************************************** */

// int	preprocess_heredocs(t_command *commands, t_global *global)
// {
// 	t_command	*current;
// 	t_list		*redirect_node;
// 	t_redirect	*redirect;

// 	(void)global;
// 	current = commands;
// 	while (current)
// 	{
// 		if (current->redirections)
// 		{
// 			redirect_node = current->redirections;
// 			while (redirect_node)
// 			{
// 				redirect = (t_redirect *)redirect_node->content;
// 				if (redirect && redirect->type == T_HEREDOC)
// 				{
// 					// Heredoc'u main process'te işle
// 					redirect->fd = handle_heredoc(redirect);
// 					if (redirect->fd == -1)
// 						return (-1);
// 				}
// 				redirect_node = redirect_node->next;
// 			}
// 		}
// 		current = current->next;
// 	}
// 	// close(redirect->fd);
// 	return (0);
// }

// // int	execute_pipeline(t_command *commands, t_global *global)
// // {
// // 	t_command	*current;
// // 	int			pipe_fd[2];
// // 	int			prev_fd;
// // 	int			last_status;
// // 	pid_t		*pids;
// // 	int			cmd_count;
// // 	int			i;

// // 	// Pre-process heredocs in main process BEFORE forking
// // 	if (preprocess_heredocs(commands, global) == -1)
// // 		return (1);

// // 	// Count commands
// // 	current = commands;
// // 	cmd_count = 0;
// // 	while (current)
// // 	{
// // 		cmd_count++;
// // 		current = current->next;
// // 	}
	
// // 	pids = halloc(sizeof(pid_t) * cmd_count);
// // 	if (!pids)
// // 		return (1);
	
// // 	current = commands;
// // 	prev_fd = 0;
// // 	i = 0;
// // 	last_status = 0;
	
// // 	while (current && i < cmd_count)
// // 	{
// // 		if (current->next && pipe(pipe_fd) == -1)
// // 		{
// // 			perror("pipe");
// // 			return (1);
// // 		}
		
// // 		pids[i] = execute_pipeline_command_async(current, global, prev_fd, pipe_fd);
// // 		if (pids[i] == -1)
// // 			return (1);
		

// // 		if (prev_fd != 0)
// // 			close(prev_fd);
// // 		if (current->next)
// // 		{
// // 			close(pipe_fd[1]);
// // 			prev_fd = pipe_fd[0];
// // 		}
// // 		current = current->next;
// // 		i++;
// // 	}
// // 	// Wait for remaining children (42 norm - while)
// // 	i = 0;
// // 	while (i < cmd_count)
// // 	{
// // 		int status;
// // 		if (pids[i] > 0) // Henüz beklenmemiş
// // 		{
// // 			waitpid(pids[i], &status, 0);
// // 			if (i == cmd_count - 1)  // Last command's exit status
// // 			{
// // 				if (WIFSIGNALED(status))
// // 				{
// // 					int signal_num = WTERMSIG(status);
// // 					if (signal_num == SIGINT)
// // 						last_status = 130;
// // 					else if (signal_num == SIGQUIT)
// // 						last_status = 131;
// // 					else
// // 						last_status = 128 + signal_num;
// // 				}
// // 				else
// // 					last_status = WEXITSTATUS(status);
// // 			}
// // 		}
// // 		i++;
// // 	}

// // 	// Cleanup heredoc FDs in parent process - MUTLAKA ÇALIŞTIR
// // 	// current = commands;
// // 	// while (current)
// // 	// {
// // 	// 	if (current->redirections)
// // 	// 	{
// // 	// 		t_list *node = current->redirections;
// // 	// 		while (node)
// // 	// 		{
// // 	// 			t_redirect *redirect = (t_redirect *)node->content;
// // 	// 			if (redirect && redirect->type == T_HEREDOC && redirect->fd > 2)
// // 	// 			{
// // 	// 				printf("FAFAFADEBUG: Closing heredoc FD %d in parent\n", redirect->fd);
// // 	// 				close(redirect->fd);
// // 	// 				redirect->fd = -1;
// // 	// 			}
// // 	// 			node = node->next;
// // 	// 		}
// // 	// 	}
// // 	// 	current = current->next;
// // 	// }
	
// // 	return (last_status);
// // }

// static int	get_process_exit_status(int status)
// {
//     int	signal_num;

//     if (WIFSIGNALED(status))
//     {
//         signal_num = WTERMSIG(status);
//         if (signal_num == SIGINT)
//             return (130);
//         else if (signal_num == SIGQUIT)
//             return (131);
//         else
//             return (128 + signal_num);
//     }
//     else
//         return (WEXITSTATUS(status));
// }

// static int	wait_for_all_pipeline_processes(pid_t *pids, int cmd_count)
// {
//     int	i;
//     int	status;
//     int	last_status;

//     // Wait for remaining children (42 norm - while)
//     i = 0;
//     last_status = 0;
//     while (i < cmd_count)
//     {
//         if (pids[i] > 0) // Henüz beklenmemiş
//         {
//             waitpid(pids[i], &status, 0);
//             if (i == cmd_count - 1)  // Last command's exit status
//             {
//                 last_status = get_process_exit_status(status);
//             }
//         }
//         i++;
//     }
//     return (last_status);
// }

// static int	execute_all_pipeline_commands(t_command *commands, t_global *global, pid_t *pids, int cmd_count)
// {
//     t_command	*current;
//     int			pipe_fd[2];
//     int			prev_fd;
//     int			i;

//     current = commands;
//     prev_fd = 0;
//     i = 0;
//     while (current && i < cmd_count)
//     {
//         if (current->next && pipe(pipe_fd) == -1)
//         {
//             perror("pipe");
//             return (-1);
//         }
//         pids[i] = execute_pipeline_command_async(current, global, prev_fd, pipe_fd);
//         if (pids[i] == -1)
//             return (-1);
//         if (prev_fd != 0)
//             close(prev_fd);
//         if (current->next)
//         {
//             close(pipe_fd[1]);
//             prev_fd = pipe_fd[0];
//         }
//         current = current->next;
//         i++;
//     }
//     return (0);
// }

// static int	count_pipeline_commands(t_command *commands)
// {
//     t_command	*current;
//     int			cmd_count;

//     current = commands;
//     cmd_count = 0;
//     while (current)
//     {
//         cmd_count++;
//         current = current->next;
//     }
//     return (cmd_count);
// }

// int	execute_pipeline(t_command *commands, t_global *global)
// {
//     pid_t	*pids;
//     int		cmd_count;
//     int		last_status;

//     // Pre-process heredocs in main process BEFORE forking
//     if (preprocess_heredocs(commands, global) == -1)
//         return (1);
//     // Count commands
//     cmd_count = count_pipeline_commands(commands);
//     pids = halloc(sizeof(pid_t) * cmd_count);
//     if (!pids)
//         return (1);
//     // Execute all pipeline commands
//     if (execute_all_pipeline_commands(commands, global, pids, cmd_count) == -1)
//         return (1);
//     // Wait for all processes and get last status
//     last_status = wait_for_all_pipeline_processes(pids, cmd_count);
//     // Cleanup heredoc FDs in parent process - MUTLAKA ÇALIŞTIR
//     // current = commands;
//     // while (current)
//     // {
//     // 	if (current->redirections)
//     // 	{
//     // 		t_list *node = current->redirections;
//     // 		while (node)
//     // 		{
//     // 			t_redirect *redirect = (t_redirect *)node->content;
//     // 			if (redirect && redirect->type == T_HEREDOC && redirect->fd > 2)
//     // 			{
//     // 				printf("FAFAFADEBUG: Closing heredoc FD %d in parent\n", redirect->fd);
//     // 				close(redirect->fd);
//     // 				redirect->fd = -1;
//     // 			}
//     // 			node = node->next;
//     // 		}
//     // 	}
//     // 	current = current->next;
//     // }
//     return (last_status);
// }
