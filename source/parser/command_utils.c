/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 00:11:02 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/28 00:11:15 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

char	**convert_list_to_array(t_list *args_list)
{
	char	**args_array;
	int		size;
	int		i;
	t_list	*current;

	size = ft_lstsize(args_list);
	args_array = halloc(sizeof(char *) * (size + 1));
	if (!args_array)
		return (NULL);
	i = 0;
	current = args_list;
	while (current)
	{
		args_array[i] = ft_strdup((char *)current->content);
		current = current->next;
		i++;
	}
	args_array[i] = NULL;
	return (args_array);
}

void	append_command_to_chain(t_command *head, t_command *new_cmd)
{
	t_command	*current;

	current = head;
	while (current->next)
		current = current->next;
	current->next = new_cmd;
}

void	add_redirect_to_command(t_command *cmd, t_redirect *redirect)
{
	ft_lstadd_back(&cmd->redirections, ft_lstnew(redirect));
}
