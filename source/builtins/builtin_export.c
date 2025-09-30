/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:26:26 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:26:41 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../lib/minishell.h"

static void	handle_single_assignment(char *arg, t_global *global)
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

static void	handle_split_assignment(char **args, int i, t_global *global)
{
	char	*full_arg;
	char	*equal_sign;
	char	*key;
	char	*value;

	full_arg = ft_strjoin(args[i], args[i + 1]);
	equal_sign = ft_strchr(full_arg, '=');
	if (equal_sign)
	{
		*equal_sign = '\0';
		key = full_arg;
		value = equal_sign + 1;
		set_env_var(global, key, value);
		*equal_sign = '=';
	}
}

static int	process_export_argument(char **args, int i, t_global *global)
{
	// Handle case: test= "ho selam" -> test=ho selam
	if (args[i][ft_strlen(args[i]) - 1] == '=' && args[i + 1])
	{
		handle_split_assignment(args, i, global);
		return (i + 2);
	}
	else
	{
		handle_single_assignment(args[i], global);
		return (i + 1);
	}
}

int	builtin_export(char **args, t_global *global)
{
	int i;

	if (!args[1])
	{
		print_export_env(global->env_list);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		i = process_export_argument(args, i, global);
	}
	return (0);
}