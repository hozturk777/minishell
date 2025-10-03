/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:17:31 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:38:23 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>

static char	*extract_inner_content(char *str, int len)
{
	char	*result;
	int		i;
	int		j;

	result = halloc(len - 1);
	if (!result)
		return (NULL);
	i = 1;
	j = 0;
	while (i < len - 1)
	{
		result[j] = str[i];
		i++;
		j++;
	}
	result[j] = '\0';
	return (result);
}

char	*remove_quotes(char *str)
{
	int	len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len < 2)
		return (ft_strdup(str));
	if ((str[0] == '"' && str[len - 1] == '"') || (str[0] == '\'' && str[len
			- 1] == '\''))
		return (extract_inner_content(str, len));
	return (ft_strdup(str));
}

int	builtin_exit(char **args)
{
	int	exit_code;

	exit_code = 0;
	printf("exit\n");
	int i;
	i = 0;
	if (args[1])
	{
		while(args[1][i])
		{
			while (args[1][i] == '+' || args[1][i] == '-')
				i++;
			if (!ft_isdigit(args[1][i]))
			{
				printf("minishell: exit: %s: numeric argument required\n", args[1]);
				cleanup_and_exit();
				exit(2);
			}
			i++;
		}
		exit_code = ft_atoi(args[1]);
		if (args[2])
		{
			printf("minishell: exit: too many arguments\n");
			return (0);
		}
	}
	cleanup_and_exit();
	exit(exit_code);
}

int	builtin_env(t_env *env_list)
{
	t_env		*current;
	t_global	*global;

	current = env_list;
	global = get_global();
	if (global->commands->args[1] && global->commands->args[1][0] == '-')
	{
		printf("env: invalid option -- '%c'\n", global->commands->args[1][1]);
		return (125);
	}
	else if (global->commands->args[1])
	{
		printf("env: '%s': No such file or directory\n", global->commands->args[1]);	
		return (127);	
	}
	while (current)
	{
		if (current->value)
			printf("%s=%s\n", current->key, current->value);
		current = current->next;
	}
	return (0);
}
