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

int	preprocess_heredocs_utils(t_command *current)
{
	t_list		*redirect_node;
	t_redirect	*redirect;

	redirect_node = current->redirections;
	while (redirect_node)
	{
		redirect = (t_redirect *)redirect_node->content;
		if (redirect && redirect->type == T_HEREDOC)
		{
			redirect->fd = preprocess_handle_heredoc(redirect);
			if (redirect->fd == -1)
				return (1);
		}
		redirect_node = redirect_node->next;
	}
	return (0);
}

void	setup_redirections_check(t_command *cmd, int *originals)
{
	if (setup_redirections(cmd))
	{
		close_unused_heredoc_fds(cmd);
		close(originals[0]);
		close(originals[1]);
		cleanup_and_exit();
		exit(2);
	}
	close_unused_heredoc_fds(cmd);
	close(originals[0]);
	close(originals[1]);
	close_all_heredoc_fds();
}

void	is_builtin_check(t_command *cmd,
		t_global *global,
		int *originals, int *exit_num)
{
	if (is_builtin(cmd->args[0]))
	{
		if (heredoc_check(cmd)
			&& !redirect_check(cmd)
			&& !isatty(STDOUT_FILENO))
		{
			cleanup_and_exit();
			exit(0);
		}
		else
		{
			execute_builtin(cmd, global, originals, 0);
			*exit_num = global->exit_status;
			cleanup_and_exit();
			exit(*exit_num);
		}
	}
}

void	cleanup_and_close(int *originals)
{
	close(originals[0]);
	close(originals[1]);
	cleanup_and_exit();
}

void	check_path(t_command *cmd, t_list *node, int *originals)
{
	t_redirect	*redirect;

	while (node)
	{
		redirect = (t_redirect *)node->content;
		if (redirect->type == T_HEREDOC)
		{
			printf("minishell: %s: command not found\n", cmd->args[0]);
			cleanup_and_close(originals);
			exit(0);
		}
		node = node->next;
	}
	printf("minishell: %s: command not found\n", cmd->args[0]);
	cleanup_and_close(originals);
	exit(127);
}
