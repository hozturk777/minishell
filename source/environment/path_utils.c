/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/09 19:25:17 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

/* ************************************************************************** */
/*                            PATH RESOLUTION                                */
/* ************************************************************************** */

char	*find_command_path(char *command, t_env *env_list)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (!command)
		return (NULL);
	if (ft_strchr(command, '/'))
	{
		if (access(command, F_OK) == 0) // access dosya var mı check için 
			return (ft_strdup(command));
		return (NULL);
	}
	path_env = get_env_value(env_list, "PATH"); // PATH env buluyor
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':'); // içeriğini ':''dan bölüyor
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = build_full_path(paths[i], command);
		if (full_path && access(full_path, F_OK) == 0) // path'in yanına eklenen command var mı check?
		{
			free_string_array(paths);
			return (full_path);
		}
		if (full_path)
			free(full_path);
		i++;
	}
	free_string_array(paths);
	return (NULL); // eğer path'de bulunmayan bir command girildiyse null dönüyor
}

/* ************************************************************************** */
/*                            HELPER FUNCTIONS                               */
/* ************************************************************************** */

char	*build_full_path(char *dir, char *command)
{
	char	*temp;
	char	*full_path;

	if (!dir || !command)
		return (NULL);
	temp = ft_strjoin(dir, "/"); // ':' ayrılan pathlerin yanına slash ekliyor
	if (!temp)
		return (NULL);
	full_path = ft_strjoin(temp, command); // Slashden sonrasına gelen command'ı ekliyor
	free(temp);
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

void	free_string_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

char	**env_list_to_array(t_env *env_list)
{
	t_env	*current;
	char	**env_array;
	char	*temp;
	int		count;
	int		i;

	count = count_env_nodes(env_list);
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	current = env_list;
	i = 0;
	while (current && i < count)
	{
		temp = ft_strjoin(current->key, "=");
		if (temp)
		{
			env_array[i] = ft_strjoin(temp, current->value);
			free(temp);
		}
		else
			env_array[i] = NULL;
		current = current->next;
		i++;
	}
	env_array[i] = NULL;
	return (env_array);
}

int	count_env_nodes(t_env *env_list)
{
	t_env	*current;
	int		count;

	current = env_list;
	count = 0;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}
