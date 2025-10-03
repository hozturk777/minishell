/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:34:28 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:35:08 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>

void	update_pwd_env(t_global *global)
{
	char	*cwd;

	cwd = add_garbage(getcwd(NULL, 0));
	if (cwd)
	{
		set_env_var(global, "PWD", cwd);
	}
}

void	print_export_env(t_env *env_list)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (current->value && ft_strlen(current->value) >= 0)
			printf("declare -x %s=\"%s\"\n", current->key, current->value);
		else
			printf("declare -x %s\n", current->key);
		current = current->next;
	}
}

void	set_env_var(t_global *global, char *key, char *value)
{
	t_env	*existing;
	t_env	*new_node;

	existing = find_env_node(global->env_list, key);
	if (existing)
	{
		existing->value = ft_strdup(value);
	}
	else
	{
		new_node = create_env_node(key, value);
		if (new_node)
			add_env_node(&global->env_list, new_node);
	}
}

void	unset_env_var(t_global *global, char *key)
{
	t_env	*current;
	t_env	*prev;

	current = global->env_list;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				global->env_list = current->next;
			return ;
		}
		prev = current;
		current = current->next;
	}
}
