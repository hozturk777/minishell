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

