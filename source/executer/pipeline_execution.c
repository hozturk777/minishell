/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_execution.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 20:50:46 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/29 20:51:45 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

static int	get_process_exit_status(int status)
{
	int	signal_num;

	if (WIFSIGNALED(status))
	{
		signal_num = WTERMSIG(status);
		if (signal_num == SIGINT)
			return (130);
		else if (signal_num == SIGQUIT)
			return (131);
		else
			return (128 + signal_num);
	}
	else
		return (WEXITSTATUS(status));
}

static int	wait_for_all_pipeline_processes(pid_t *pids, int cmd_count)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < cmd_count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (i == cmd_count - 1)
			{
				last_status = get_process_exit_status(status);
			}
		}
		i++;
	}
	return (last_status);
}

static int	execute_all_pipeline_commands(t_command *commands,
	t_global *global,
	pid_t *pids, int cmd_count)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_fd;
	int			i;

	current = commands;
	prev_fd = 0;
	i = 0;
	while (current && i < cmd_count)
	{
		if (current->next && pipe(pipe_fd) == -1)
		{
			perror("pipe");
			return (-1);
		}
		pids[i] = execute_pipeline_command_async(current,
				global, prev_fd, pipe_fd);
		if (pids[i] == -1)
			return (-1);
		pipeline_close_helper(&prev_fd, &current, pipe_fd);
		i++;
	}
	return (0);
}

static int	count_pipeline_commands(t_command *commands)
{
	t_command	*current;
	int			cmd_count;

	current = commands;
	cmd_count = 0;
	while (current)
	{
		cmd_count++;
		current = current->next;
	}
	return (cmd_count);
}

int	execute_pipeline(t_command *commands, t_global *global)
{
	pid_t	*pids;
	int		cmd_count;
	int		last_status;

	if (preprocess_heredocs(commands) == -1)
		return (1);
	cmd_count = count_pipeline_commands(commands);
	pids = halloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		return (1);
	if (execute_all_pipeline_commands(commands, global, pids, cmd_count) == -1)
		return (1);
	last_status = wait_for_all_pipeline_processes(pids, cmd_count);
	return (last_status);
}
