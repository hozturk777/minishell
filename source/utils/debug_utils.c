/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:37:10 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/26 14:38:26 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

void	print_commands_debug(t_command *commands)
{
	t_command	*current;
	t_list		*redirect_node;
	t_redirect	*redirect;
	int			cmd_count;
	int			i;

	current = commands;
	cmd_count = 1;
	while (current)
	{
		printf("Command %d:\n", cmd_count);
		// Print arguments
		printf("  Args: ");
		if (current->args)
		{
			i = 0;
			while (current->args[i])
			{
				printf("$%s$", current->args[i]);
				if (current->args[i + 1])
					printf(", ");
				i++;
			}
		}
		else
		{
			printf("(none)");
		}
		printf("\n");
		// Print redirections
		printf("  Redirections: ");
		redirect_node = current->redirections;
		if (redirect_node)
		{
			printf("\n");
			while (redirect_node)
			{
				redirect = (t_redirect *)redirect_node->content;
				printf("    ");
				switch (redirect->type)
				{
				case T_REDIRECT_IN:
					printf("< $%s$", redirect->filename);
					break ;
				case T_REDIRECT_OUT:
					printf("> $%s$", redirect->filename);
					break ;
				case T_APPEND:
					printf(">> $%s$", redirect->filename);
					break ;
				case T_HEREDOC:
					printf("<< $%s$", redirect->filename);
					break ;
				default:
					printf("Unknown redirect");
				}
				printf("\n");
				redirect_node = redirect_node->next;
			}
		}
		else
		{
			printf("(none)\n");
		}
		printf("  Pipe: %s\n", current->next ? "YES" : "NO");
		printf("  Pid: %d\n", current->pid);
		printf("\n");
		current = current->next;
		cmd_count++;
	}
}



t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = halloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirections = NULL;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->pid = -1;
	cmd->next = NULL;
	return (cmd);
}
