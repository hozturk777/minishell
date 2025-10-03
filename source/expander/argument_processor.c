/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument_processor.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 19:40:55 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/29 19:41:09 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"



void	expand_command_args(t_command *cmd, t_global *global)
{
	int		i;
	char	*expanded;
	char	*clean_str;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		if (is_double_quoted_literal(cmd->args[i]))
		{
			expanded = expand_with_quotes(cmd->args[i], global);
			if (expanded)
				cmd->args[i] = expanded;
		}
		else if (is_single_quoted_literal(cmd->args[i]))
		{
			clean_str = extract_single_quote_content(cmd->args[i]);
			cmd->args[i] = clean_str;
		}
		else if (needs_expansion(cmd->args[i]))
		{
			expanded = expand_with_quotes(cmd->args[i], global);
			if (expanded)
				cmd->args[i] = expanded;
		}
		else
		{
			expanded = remove_outer_quotes(cmd->args[i]);
			if (expanded)
				cmd->args[i] = expanded;
		}
		i++;
	}
	filter_empty_args(cmd);
}

void	filter_empty_args(t_command *cmd)
{
	int		i;
	int		j;
	char	**new_args;
	int		count;

	if (!cmd || !cmd->args)
		return ;
	count = count_non_empty_args(cmd->args);
	new_args = halloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return ;
	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] != '\0')
		{
			new_args[j] = cmd->args[i];
			cmd->args[i] = NULL;
			j++;
		}
		i++;
	}
	new_args[j] = NULL;
	cmd->args = new_args;
}

int	count_non_empty_args(char **args)
{
	int	i;
	int	count;

	if (!args)
		return (0);
	i = 0;
	count = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	return (count);
}
