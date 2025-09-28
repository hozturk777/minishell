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

/**
 * SIGINT (Ctrl+C) sinyal handler'ı
 * - Interactive modda: Yeni prompt göster
 * - Child process'te: Process'i sonlandır
 */
// void	sigint_handler(int sig)
// {
// 	(void)sig;
// 	t_redirect *redirect;
// 	t_global *g_global;

// 	g_global = get_global();

// 	if (g_global && g_global->in_child)
// 	{
// 		// Child process'te - default davranış
// 		//buraya fd close gelmesi gerekioyor !!!!!!!!!!!!!!

// 		while (g_global->commands->redirections)
// 		{
// 			redirect = (t_redirect *)g_global->commands->redirections->content;
// 			// printf("REDİRECT: $%d$\n", redirect->fd);
// 			if (redirect->fd > 0)
// 			{
// 				close(redirect->fd);
// 			}
// 			g_global->commands->redirections = g_global->commands->redirections->next;
// 		}
// 		clear_garbage();
// 		exit(130); // 128 + SIGINT signal number (2)
// 	}
// 	else
// 	{
// 		// Parent process (interactive shell) - yeni satır ve prompt
// 		write(STDOUT_FILENO, "\n", 1);
// 		rl_on_new_line();
// 		rl_replace_line("", 0);
// 		rl_redisplay();

// 		if (g_global)
// 			g_global->exit_status = 130;
// 	}
// }

#include "../../lib/minishell.h"

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
		clear_garbage();
		exit(130);
	}
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	if (g_global)
		g_global->exit_status = 130;
}
