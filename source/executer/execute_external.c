/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_external.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 20:52:51 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 12:44:31 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

static void	external_child_process(t_command *cmd, t_global *global, char *path)
{
	setup_child_signals();
	global->in_child = 2;
	if (setup_redirections(cmd))
	{
		cleanup_and_exit();
		exit(2);
	}
	if (!path)
	{
		cleanup_and_exit();
		exit(127);
	}
	setup_child_signals_sigdfl();
	execve(path, cmd->args, env_list_to_array(global->env_list));
	perror("execve");
	cleanup_and_exit();
	exit(127);
}

void	print_signal_message(int signal_num)
{
	if (signal_num == SIGQUIT)
		write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
	else if (signal_num == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
}

int	wait_for_external_process(pid_t pid)
{
	int	status;
	int	signal_num;

	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		signal_num = WTERMSIG(status);
		print_signal_message(signal_num);
		if (signal_num == SIGINT)
			return (130);
		else if (signal_num == SIGQUIT)
			return (131);
		else
			return (128 + signal_num);
	}
	return (WEXITSTATUS(status));
}

static void	external_child(t_command *cmd, t_global *global, char *path)
{
	external_child_process(cmd, global, path);
	cleanup_and_exit();
	exit (127);
}

int	execute_external_command(t_command *cmd, t_global *global)
{
	char	*path;
	pid_t	pid;

	path = find_command_path(cmd->args[0], global->env_list);
	pid = fork();
	if (pid == 0)
		external_child(cmd, global, path);
	else if (pid > 0)
	{
		global->exit_status = wait_for_external_process(pid);
		if (!path)
		{
			printf("minishell: %s: command not found\n", cmd->args[0]);
			return (127);
		}
		return (global->exit_status);
	}
	else
	{
		perror("fork");
		free(path);
		clear_garbage();
		return (1);
	}
	return (0);
}
