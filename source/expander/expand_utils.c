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

void	expand_double_quotes(t_command *cmd, t_global *global, int *i)
{
	char	*expanded;

	expanded = expand_with_quotes(cmd->args[*i], global);
	if (expanded)
		cmd->args[*i] = expanded;
}

void	expand_single_quotes(t_command *cmd, int *i)
{
	char	*clean_str;

	clean_str = extract_single_quote_content(cmd->args[*i]);
	cmd->args[*i] = clean_str;
}

void	need_expansion_utils(t_command *cmd, t_global *global, int *i)
{
	char	*expanded;

	expanded = expand_with_quotes(cmd->args[*i], global);
	if (expanded)
		cmd->args[*i] = expanded;
}
