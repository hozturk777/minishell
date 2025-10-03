/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/30 20:31:54 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static char	*search_in_path_directories(char *path_env, char *command)
{
	char	**paths;
	char	*full_path;
	int		i;

	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = build_full_path(paths[i], command);
		if (full_path && access(full_path, F_OK) == 0)
		{
			return (full_path);
		}
		if (full_path)
			i++;
	}
	return (NULL);
}

char	*find_command_path(char *command, t_env *env_list)
{
	char	*path_env;

	if (!command)
		return (NULL);
	if (ft_strchr(command, '/'))
	{
		if (access(command, F_OK) == 0)
			return (ft_strdup(command));
		return (NULL);
	}
	path_env = get_env_value(env_list, "PATH");
	if (!path_env)
		return (NULL);
	return (search_in_path_directories(path_env, command));
}

char	*build_full_path(char *dir, char *command)
{
	char	*temp;
	char	*full_path;

	if (!dir || !command)
		return (NULL);
	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	full_path = ft_strjoin(temp, command);
	return (full_path);
}

char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}


char	**env_list_to_array(t_env *env_list)
{
	t_env	*current;
	char	**env_array;
	char	*temp;
	int		count;
	int		i;

	count = count_env_nodes(env_list);
	env_array = halloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	current = env_list;
	i = 0;
	while (current && i < count)
	{
		temp = ft_strjoin(current->key, "=");
		if (temp)
			env_array[i] = ft_strjoin(temp, current->value);
		else
			env_array[i] = NULL;
		current = current->next;
		i++;
	}
	env_array[i] = NULL;
	return (env_array);
}
