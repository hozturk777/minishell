/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:56:33 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/05 20:56:36 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static char	*process_character(char *input, int *i,
	t_global *global, int *quote_state)
{
	char	*temp;

	if (input[*i] == '$' && !quote_state[0])
		temp = handle_dollar_expansion(input, i, global);
	else if (input[*i] == '\'' && quote_state[1])
		temp = handle_regular_char(input, i);
	else if (input[*i] == '"' && quote_state[0])
		temp = handle_regular_char(input, i);
	else if (input[*i] != '\'' && input[*i] != '"')
		temp = handle_regular_char(input, i);
	else
	{
		(*i)++;
		return (NULL);
	}
	return (temp);
}

static void	update_quote_state(char c, int *quote_state)
{
	if (c == '\'' && !quote_state[1])
		quote_state[0] = !quote_state[0];
	else if (c == '"' && !quote_state[0])
		quote_state[1] = !quote_state[1];
}

char	*expand_with_quotes(char *input, t_global *global)
{
	char	*result;
	char	*temp;
	int		i;
	int		quote_state[2];

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	quote_state[0] = 0;
	quote_state[1] = 0;
	while (input[i])
	{
		update_quote_state(input[i], quote_state);
		temp = process_character(input, &i, global, quote_state);
		if (!temp)
			continue ;
		result = ft_strjoin(result, temp);
		if (!result)
			return (NULL);
	}
	return (result);
}

char	*expand_with_heredoc(char *input, t_global *global)
{
	char	*result;
	char	*temp;
	int		i;

	result = ft_strdup("");
	i = 0;
	while (input[i])
	{
		if (input[i] == '$')
			temp = handle_dollar_expansion(input, &i, global);
		else
		{
			result = ft_strjoin_char(result, input[i]);
			i++;
			continue ;
		}
		result = ft_strjoin(result, temp);
		if (!result)
			return (NULL);
	}
	return (result);
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
