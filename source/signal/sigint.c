/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sigint.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 23:12:29 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/27 23:12:30 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <readline/readline.h>
#include <unistd.h>

void	sigint_handler_child_cleanup(t_command *cmd)
{
	t_redirect	*redirect;
	t_list		*node;

	node = cmd->redirections;
	while (node)
	{
		redirect = (t_redirect *)node->content;
		if (redirect->fd > 0)
			close(redirect->fd);
		node = node->next;
	}
}

void	sigint_handler(int sig)
{
	t_global	*g_global;

	(void)sig;
	g_global = get_global();
	if (g_global && g_global->in_child)
	{
		if (g_global->commands)
			sigint_handler_child_cleanup(g_global->commands);
		cleanup_and_exit();
		exit(130);
	}
	close_all_heredoc_fds();
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	if (g_global)
		g_global->exit_status = 130;
}
