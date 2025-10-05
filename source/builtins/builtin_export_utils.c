/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:26:26 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/04 19:13:41 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

void	handle_single_assignment(char *arg, t_global *global)
{
	char	*equal_sign;
	char	*key;
	char	*value;

	equal_sign = ft_strchr(arg, '=');
	if (equal_sign)
	{
		*equal_sign = '\0';
		key = arg;
		value = equal_sign + 1;
		set_env_var(global, key, value);
		*equal_sign = '=';
	}
	else
	{
		key = arg;
		set_env_var(global, key, NULL);
	}
}

void	handle_split_assignment(char **args, int i, t_global *global)
{
	char	*key;
	int		key_len;

	key_len = ft_strlen(args[i]) - 1;
	key = ft_substr(args[i], 0, key_len);
	if (!key)
		return ;
	set_env_var(global, key, "");
}

int	is_valid_key_char(char *arg, char *equal_pos)
{
	int	h;

	h = 0;
	while (arg[h] && (equal_pos == NULL || &arg[h] < equal_pos))
	{
		if (arg[h] == '-')
			return (0);
		h++;
	}
	return (1);
}

int	check_token_at_index(t_global *global, int index)
{
	t_list		*current;
	t_token_new	*token;
	int			count;

	if (!global || !global->tokens)
		return (0);
	current = global->tokens;
	count = 0;
	while (current && count < index)
	{
		current = current->next;
		count++;
	}
	if (current)
	{
		token = (t_token_new *)current->content;
		if (token && (token->type == T_DOUBLE_QUOTE
				|| token->type == T_SINGLE_QUOTE))
			return (1);
	}
	return (0);
}

char	*merge_quoted_args(char **args, int s_index,
	int *e_index, t_global *global)
{
	char	*result;
	char	*temp;
	int		i;

	result = ft_strdup(args[s_index]);
	if (!result)
		return (NULL);
	i = s_index + 1;
	if (args[i] && check_token_at_index(global, i))
	{
		temp = ft_strjoin(result, args[i]);
		if (!temp)
			return (NULL);
		result = temp;
		*e_index = i;
	}
	else
		*e_index = s_index;
	return (result);
}
