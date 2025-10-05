/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/30 20:01:53 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <unistd.h>

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

static t_env	*create_minimal_env(void)
{
	t_env	*env_list;
	t_env	*new_node;
	char	*cwd;

	cwd = add_garbage(getcwd(NULL, 0));
	if (cwd)
	{
		new_node = create_env_node("PWD", cwd);
		if (new_node)
			add_env_node(&env_list, new_node);
	}
	new_node = create_env_node("SHLVL", "0");
	if (new_node)
		add_env_node(&env_list, new_node);
	new_node = create_env_node("_", "./minishell");
	if (new_node)
		add_env_node(&env_list, new_node);
	return (env_list);
}

static t_env	*process_envp_entries(char **envp)
{
	t_env	*env_list;
	t_env	*new_node;
	char	*key;
	char	*value;
	int		i;

	env_list = NULL;
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
		i++;
	}
	return (env_list);
}

t_env	*init_env_from_envp(char **envp)
{
	t_env	*env_list;

	env_list = NULL;
	if (!envp || !envp[0])
		return (create_minimal_env());
	else
		return (process_envp_entries(envp));
}
