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
#include <stdio.h>

static int	process_export_argument(char **args, int i, t_global *global)
{
	int	arg_len;

	arg_len = ft_strlen(args[i]);
	if (arg_len > 0 && args[i][arg_len - 1] == '=')
	{
		handle_split_assignment(args, i, global);
		return (i + 1);
	}
	else
	{
		handle_single_assignment(args[i], global);
		return (i + 1);
	}
}

static int	is_valid_export(char *args, char *equal_pos)
{
	if (!is_valid_key_char(args, equal_pos))
	{
		printf("minishell: export: -%c: invalid option\n", args[1]);
		return (1);
	}
	return (0);
}

int	is_equal_export(char *equal_pos, char *merged_arg, t_global *global)
{
	equal_pos = ft_strchr(merged_arg, '=');
	if (!is_valid_key_char(merged_arg, equal_pos))
	{
		printf("export: -%c: invalid option\n", merged_arg[1]);
		return (1);
	}
	handle_single_assignment(merged_arg, global);
	return (0);
}

static int	check_and_print_export(char *args, t_global *global, int *i)
{
	*i = 1;
	if (!args)
	{
		print_export_env(global->env_list);
		return (1);
	}
	return (0);
}

int	builtin_export(char **args, t_global *global)
{
	int		i;
	int		end_index;
	char	*equal_pos;
	int		result;

	if (check_and_print_export(args[1], global, &i))
		return (0);
	while (args[i])
	{
		equal_pos = ft_strchr(args[i], '=');
		if (equal_pos && args[i + 1] && check_token_at_index(global, i + 1))
		{
			result = handle_quoted_export(args, i, global, &end_index);
			if (result != 0)
				return (result);
			i = end_index + 1;
		}
		else
		{
			if (is_valid_export(args[i], equal_pos))
				return (2);
			i = process_export_argument(args, i, global);
		}
	}
	return (0);
}
