/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 12:00:00 by hsyn              #+#    #+#             */
/*   Updated: 2025/08/22 12:00:00 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

char	*expand_with_quotes(char *input, t_global *global)
{
	char	*result;
	char	*temp;
	int		i;
	int		quote_state[2];

	result = ft_strdup("");
	i = 0;
	quote_state[0] = 0;  // single quote
	quote_state[1] = 0;  // double quote
	while (input[i])
	{
		update_quote_state(input[i], quote_state);
		if (input[i] == '$' && !quote_state[0])
			temp = handle_dollar_expansion(input, &i, global);
		else if (input[i] != '\'' && input[i] != '"')
			temp = handle_regular_char(input, &i);
		else
		{
			i++;
			continue ;
		}
		result = join_and_free(result, temp);
		if (!result)
			return (NULL);
	}
	return (result);
}

void	update_quote_state(char c, int *quote_state)
{
	if (c == '\'' && !quote_state[1])
		quote_state[0] = !quote_state[0];
	else if (c == '"' && !quote_state[0])
		quote_state[1] = !quote_state[1];
}

char	*remove_outer_quotes(char *input)
{
	int		len;
	char	*result;

	if (!input)
		return (NULL);
	len = ft_strlen(input);
	if (len < 2)
		return (ft_strdup(input));
	if ((input[0] == '\'' && input[len - 1] == '\'')
		|| (input[0] == '"' && input[len - 1] == '"'))
	{
		result = ft_substr(input, 1, len - 2);
		return (result);
	}
	return (ft_strdup(input));
}

int	needs_expansion(char *str)
{
	int	i;
	int	in_single_quote;

	if (!str)
		return (0);
	i = 0;
	in_single_quote = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_single_quote)
			in_single_quote = 1;
		else if (str[i] == '\'' && in_single_quote)
			in_single_quote = 0;
		else if (str[i] == '$' && !in_single_quote)
			return (1);
		i++;
	}
	return (0);
}

void	expand_command_args(t_command *cmd, t_global *global)
{
	int		i;
	char	*expanded;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		if (needs_expansion(cmd->args[i]))
		{
			expanded = expand_with_quotes(cmd->args[i], global);
			if (expanded)
			{
				free(cmd->args[i]);
				cmd->args[i] = expanded;
			}
		}
		else
		{
			expanded = remove_outer_quotes(cmd->args[i]);
			if (expanded)
			{
				free(cmd->args[i]);
				cmd->args[i] = expanded;
			}
		}
		i++;
	}
}
