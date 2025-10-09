/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_built.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:36:19 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:36:41 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>

char	*find_existing_parent(char *path)
{
	char	*current_path;
	char	*last_slash;
	char	*temp;

	if (!path)
		return (NULL);
	current_path = ft_strdup(path);
	if (!current_path)
		return (NULL);
	while (current_path && ft_strlen(current_path) > 1)
	{
		if (access(current_path, F_OK) == 0)
			return (current_path);
		last_slash = ft_strrchr(current_path, '/');
		if (!last_slash || last_slash == current_path)
		{
			return (ft_strdup("/"));
		}
		temp = ft_substr(current_path, 0, last_slash - current_path);
		current_path = temp;
	}
	return (ft_strdup("/"));
}

static char	*process_path_logic(char *clean_pwd, char *path)
{
	char	*last_slash;
	char	*temp;
	char	*resolved;

	if (ft_strcmp(path, ".") == 0)
		return (ft_strdup(clean_pwd));
	if (ft_strcmp(path, "..") == 0)
	{
		last_slash = ft_strrchr(clean_pwd, '/');
		if (last_slash && last_slash != clean_pwd)
			return (ft_substr(clean_pwd, 0, last_slash - clean_pwd));
		return (ft_strdup("/"));
	}
	temp = ft_strjoin(clean_pwd, "/");
	if (!temp)
		return (NULL);
	resolved = ft_strjoin(temp, path);
	return (resolved);
}

static char	*clean_trailing_slash(char *current_pwd)
{
	int	len;

	len = ft_strlen(current_pwd);
	if (len > 1 && current_pwd[len - 1] == '/')
		return (ft_substr(current_pwd, 0, len - 1));
	else
		return (ft_strdup(current_pwd));
}

char	*resolve_logical_path(char *current_pwd, char *path)
{
	char	*clean_pwd;
	char	*result;

	if (!path)
		return (NULL);
	if (path[0] == '/')
		return (ft_strdup(path));
	if (!current_pwd)
		return (ft_strdup(path));
	clean_pwd = clean_trailing_slash(current_pwd);
	if (!clean_pwd)
		return (NULL);
	result = process_path_logic(clean_pwd, path);
	return (result);
}

void	print_error(void)
{
	printf("cd: error retrieving current directory: ");
	printf("getcwd: cannot access parent directories: ");
	printf("No such file or directory\n");
}
