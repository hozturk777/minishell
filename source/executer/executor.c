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
#include <stdio.h>
#include <unistd.h>

int	preprocess_heredocs(t_command *commands)
{
	t_command	*current;

	current = commands;
	while (current)
	{
		if (current->redirections)
		{
			preprocess_heredocs_utils(current);
		}
		current = current->next;
	}
	return (0);
}

void	close_unused_heredoc_fds(t_command *cmd)
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
			close(redirect->fd);
			redirect->fd = -1;
		}
		node = node->next;
	}
}

int	redirect_check(t_command *command)
{
	t_redirect	*redirect;
	t_list		*node;

	node = command->redirections;
	while (node)
	{
		redirect = (t_redirect *)node->content;
		while (redirect)
		{
			if (redirect->type != T_HEREDOC)
				return (1);
			redirect = redirect->next;
		}
		node = node->next;
	}
	return (0);
}

int	heredoc_check(t_command	*command)
{
	t_command	*command_temp;
	t_redirect	*redirect;
	t_list		*node;

	command_temp = command;
	if (!command_temp)
		return (0);
	node = command_temp->redirections;
	redirect = NULL;
	while (command_temp)
	{
		node = command_temp->redirections;
		if (node)
			redirect = (t_redirect *)node->content;
		if (redirect)
		{
			if (heredoc_check_utils(redirect))
				return (1);
		}
		command_temp = command_temp->next;
	}
	return (0);
}

pid_t	execute_pipeline_command_async(t_command *cmd, t_global *global,
	int prev_fd, int *pipe_fd)
{
	pid_t		pid;
	int			exit_num;
	int			originals[2];

	pid = fork();
	exit_num = 0;
	if (pid == 0)
	{
		setup_child_signals();
		global->in_child = 1;
		originals[0] = dup(STDIN_FILENO);
		originals[1] = dup(STDOUT_FILENO);
		setup_pipeline_fds(cmd, prev_fd, pipe_fd);
		setup_redirections_check(cmd, originals);
		is_builtin_check(cmd, global, originals, &exit_num);
		child_run_execve(cmd, originals);
	}
	else if (pid > 0)
		return (pid);
	else
	{
		perror("fork");
		return (-1);
	}
	return (0);
}
