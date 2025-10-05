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

int	heredoc_check_utils(t_redirect *redirect)
{
	while (redirect)
	{
		if (redirect->type == T_HEREDOC)
			return (1);
		redirect = redirect->next;
	}
	return (0);
}

void	child_run_execve(t_command *cmd, int *originals)
{
	t_global	*global;
	t_list		*node;
	char		*path;

	global = get_global();
	node = cmd->redirections;
	path = find_command_path(cmd->args[0], global->env_list);
	if (!path)
		check_path(cmd, node, originals);
	execve(path, cmd->args, env_list_to_array(global->env_list));
	perror("execve");
	cleanup_and_close(originals);
	exit(127);
}

void	pipeline_close_helper(int *prev_fd, t_command **current, int *pipe_fd)
{
	if (*prev_fd != 0)
		close(*prev_fd);
	if ((*current)->next)
	{
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	}
	*current = (*current)->next;
}
