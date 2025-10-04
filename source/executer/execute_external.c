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

static void	handle_external_child_process(t_command *cmd, t_global *global, char *path)
{
    setup_child_signals();
    global->in_child = 2;
    if(setup_redirections(cmd))
	{
	    cleanup_and_exit();
		exit(2);
	}
    execve(path, cmd->args, env_list_to_array(global->env_list));
    perror("execve");
    cleanup_and_exit();
    exit(127);
}

static int	wait_for_external_process(pid_t pid)
{
	int	status;

    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status))
    {
        int signal_num = WTERMSIG(status);
        if (signal_num == SIGINT)
		return (130);
        else if (signal_num == SIGQUIT)
		return (131);
        else
		return (128 + signal_num);
    }
    return (WEXITSTATUS(status));
}
int	execute_external_command(t_command *cmd, t_global *global)
{
	char	*path;
	pid_t	pid;

	path = find_command_path(cmd->args[0], global->env_list);
	if (!path)
	{
		printf("minishell: %s: command not found\n", cmd->args[0]);
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		handle_external_child_process(cmd, global, path);
		exit (127);
	}
	else if (pid > 0)
	{
		return (wait_for_external_process(pid));
	}
	else
	{
		perror("fork");
		free(path);
		clear_garbage();
		return (1);
	}
}
