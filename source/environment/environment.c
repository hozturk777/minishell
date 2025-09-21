/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/21 15:44:43 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

t_env	*create_env_node(char *key, char *value)
{
	t_env	*node;

	node = (t_env *)halloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	node->value = ft_strdup(value);
	//free(key);
	//free(value);  //eklemek gerekebilir
	node->next = NULL;
	if (!node->key || !node->value)
	{
		// if (node->key)
		// 	free(node->key);
		// if (node->value)
		// 	free(node->value);
		// free(node);
		return (NULL);
	}
	return (node);
}

void	add_env_node(t_env **env_list, t_env *new_node)
{
	t_env	*current;

	if (!env_list || !new_node)
		return ;
	if (!*env_list)
	{
		*env_list = new_node;
		return ;
	}
	current = *env_list;
	while (current->next)
		current = current->next;
	current->next = new_node;
}

t_env	*find_env_node(t_env *env_list, char *key)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0
			&& ft_strlen(current->key) == ft_strlen(key))
			return (current);
		current = current->next;
	}
	return (NULL);
}

void	free_env_list(t_env *env_list)
{
	// t_env	*temp;

	// while (env_list)
	// {
	// 	temp = env_list->next;
	// 	if (env_list->key)
	// 		free(env_list->key);
	// 	if (env_list->value)
	// 		free(env_list->value);
	// 	free(env_list);
	// 	env_list = temp;
	// }
	(void)env_list;
}
