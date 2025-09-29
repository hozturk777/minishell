/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/21 13:32:41 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

/* ************************************************************************** */
/*                            CD BUILT-IN                                    */
/* ************************************************************************** */

int	builtin_cd(char **args, t_global *global)
{
	char	*path;
	char	*home;
	char	*old_pwd;
	char	*logical_path;
	char	*existing_parent;

	old_pwd = get_env_value(global->env_list, "PWD");
	if (!args[1]) // cd girildiğinde home dizinine gitmek için
	{
		home = get_env_value(global->env_list, "HOME");
		if (!home)
		{
			printf("minishell: cd: HOME not set\n");
			return (1);
		}
		path = home;
	}
	else
		path = args[1];

	if (global->commands->args[2])
	{
		printf("minishell: cd: too many arguments\n");
		return (1);
	}
	
	if (chdir(path) != 0) // chdir dizin değiştirmek için başarılı: 0 hata: -1
	{
		/* Bi önceki dizin silindiğinde çalışacak olan func  HATALI!! */
		if (ft_strcmp(path, "..") == 0 && old_pwd)
		{
			logical_path = resolve_logical_path(old_pwd, path);
			if (logical_path)
			{
				/* Find the first existing parent directory */
				existing_parent = find_existing_parent(logical_path);
				if (existing_parent)
				{
					if (chdir(existing_parent) == 0)
					{
						if (old_pwd)
							set_env_var(global, "OLDPWD", old_pwd);
						set_env_var(global, "PWD", existing_parent);
						// free(logical_path);
						// free(existing_parent);
						return (0);
					}
					// free(existing_parent);
				}
				// free(logical_path);
			}
		}
		// perror("minishell: cd: %s: ", path);
		printf("minishell: cd: %s: No such file or directory\n", path);
		return (1);
	}
	if (old_pwd) // old pwd güncellemek için
		set_env_var(global, "OLDPWD", old_pwd);
	logical_path = resolve_logical_path(old_pwd, path);
	if (logical_path) // pwd güncellemek için
	{
		set_env_var(global, "PWD", logical_path);
		// free(logical_path);
	}
	else
		update_pwd_env(global);
	return (0);
}

/* ************************************************************************** */
/*                            EXPORT BUILT-IN                                */
/* ************************************************************************** */

int	builtin_export(char **args, t_global *global)
{
	int		i;
	char	*key;
	char	*value;
	char	*equal_sign;

	if (!args[1])
	{
		print_export_env(global->env_list);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		// Handle case: test= "ho selam" -> test=ho selam
		if (args[i][ft_strlen(args[i]) - 1] == '=' && args[i + 1])
		{
			char *full_arg = ft_strjoin(args[i], args[i + 1]);
			equal_sign = ft_strchr(full_arg, '=');
			if (equal_sign)
			{
				*equal_sign = '\0';
				key = full_arg;
				value = equal_sign + 1;
				set_env_var(global, key, value);
				*equal_sign = '=';
			}
			// free(full_arg);
			i += 2; // Skip next arg since we processed it
		}
		else
		{
			equal_sign = ft_strchr(args[i], '=');
			if (equal_sign)
			{
				*equal_sign = '\0';
				key = args[i];
				value = equal_sign + 1;
				set_env_var(global, key, value);
				*equal_sign = '=';
			}
			else
			{
				key = args[i];
				set_env_var(global, key, "");
			}
			i++;
		}
	}
	return (0);
}

/* ************************************************************************** */
/*                            UNSET BUILT-IN                                 */
/* ************************************************************************** */

int	builtin_unset(char **args, t_global *global)
{
	int	i;

	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		unset_env_var(global, args[i]);
		i++;
	}
	return (0);
}

/* ************************************************************************** */
/*                            ENVIRONMENT HELPER FUNCTIONS                   */
/* ************************************************************************** */

void	update_pwd_env(t_global *global)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		set_env_var(global, "PWD", cwd);
		// free(cwd);
	}
	/* 
	 * getcwd() başarısızsa PWD'yi değiştirme
	 * Logical PWD'yi koru (bash davranışı)
	 */
}

void	print_export_env(t_env *env_list)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (current->value && ft_strlen(current->value) > 0)
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
		// free(existing->value);
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
			// free(current->key);
			// free(current->value);
			// free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

/* ************************************************************************** */
/*                            PATH RESOLUTION HELPER                         */
/* ************************************************************************** */

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
			// free(current_path);
			return (ft_strdup("/"));
		}
		temp = ft_substr(current_path, 0, last_slash - current_path);
		// free(current_path);
		current_path = temp;
	}
	// free(current_path);
	return (ft_strdup("/"));
}

char	*resolve_logical_path(char *current_pwd, char *path)
{
	char	*resolved;
	char	*temp;
	char	*last_slash;
	char	*clean_pwd;
	int		len;

	if (!path)
		return (NULL);
	if (path[0] == '/') // Neden var ?
		return (ft_strdup(path));
	if (!current_pwd)
		return (ft_strdup(path));
	
	/* Remove trailing slash from current_pwd */
	len = ft_strlen(current_pwd);
	if (len > 1 && current_pwd[len - 1] == '/')
		clean_pwd = ft_substr(current_pwd, 0, len - 1);
	else
		clean_pwd = ft_strdup(current_pwd);
	
	if (!clean_pwd)
		return (NULL);
		
	if (ft_strcmp(path, ".") == 0)
		return (clean_pwd);
	if (ft_strcmp(path, "..") == 0) // Bakılacak
	{
		last_slash = ft_strrchr(clean_pwd, '/');
		if (last_slash && last_slash != clean_pwd)
		{
			temp = ft_substr(clean_pwd, 0, last_slash - clean_pwd);
			// free(clean_pwd);
			return (temp);
		}
		// free(clean_pwd);
		return (ft_strdup("/"));
	}
	temp = ft_strjoin(clean_pwd, "/");
	// free(clean_pwd);
	if (!temp)
		return (NULL);
	resolved = ft_strjoin(temp, path);
	// free(temp);
	return (resolved);
}
