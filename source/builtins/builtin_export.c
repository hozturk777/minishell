/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:26:26 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/02 21:24:54 by hasivaci         ###   ########.fr       */
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
    char	*key;
    int		key_len;

    key_len = ft_strlen(args[i]) - 1;
    key = ft_substr(args[i], 0, key_len);
    if (!key)
        return ;
    set_env_var(global, key, "");
}

static int	is_valid_key_char(char *arg, char *equal_pos)
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


int	builtin_export(char **args, t_global *global)
{
	int i;
	char *equal_pos;

	if (!args[1])
	{
		print_export_env(global->env_list);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		equal_pos = ft_strchr(args[i], '=');
        if (!is_valid_key_char(args[i], equal_pos))
        {
            printf("export: `%s': not a valid identifier\n", args[i]);
            return (2);
        }
		i = process_export_argument(args, i, global);
	}
	return (0);
}
