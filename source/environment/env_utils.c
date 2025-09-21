/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/21 16:22:14 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static char	*extract_env_key(char *env_str)
{
	int		i;
	char	*key;

	i = 0;
	while (env_str[i] && env_str[i] != '=')
		i++;
	key = ft_substr(env_str, 0, i);
	return (key);
}

static char	*extract_env_value(char *env_str)
{
	int		i;
	char	*value;

	i = 0;
	while (env_str[i] && env_str[i] != '=')
		i++;
	if (env_str[i] == '=')
		i++;
	value = ft_strdup(&env_str[i]);
	return (value);
}

t_env	*init_env_from_envp(char **envp)
{
	t_env	*env_list;
	t_env	*new_node;
	char	*key;
	char	*value;
	int		i;

	env_list = NULL; // new_node için de başlangıç değeri gerekebilir.
	i = 0;
	while (envp[i])
	{
		key = extract_env_key(envp[i]);
		value = extract_env_value(envp[i]);
		if (key && value)
		{
			new_node = create_env_node(key, value);
			if (new_node)
				add_env_node(&env_list, new_node);
		}
		// if (key) // eklemek gerekbilir
		// 	free(key);
		// if (value)
		// 	free(value);
		i++;
	}
	return (env_list);
}
