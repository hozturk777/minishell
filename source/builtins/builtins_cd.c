/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/30 20:39:03 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>

static void	update_pwd_variables(char *old_pwd, char *path, t_global *global)
{
    char	*logical_path;

    if (old_pwd)
        set_env_var(global, "OLDPWD", old_pwd);
    logical_path = resolve_logical_path(old_pwd, path);
    if (logical_path)
    {
        set_env_var(global, "PWD", logical_path);
    }
    else
        update_pwd_env(global);
}

static int	handle_chdir_failure(char *path, char *old_pwd, t_global *global)
{
    char	*logical_path;
    char	*existing_parent;

    if (ft_strcmp(path, "..") == 0 && old_pwd)
    {
        logical_path = resolve_logical_path(old_pwd, path);
        if (logical_path)
        {
            existing_parent = find_existing_parent(logical_path);
            if (existing_parent)
            {
                if (chdir(existing_parent) == 0)
                {
                    if (old_pwd)
                        set_env_var(global, "OLDPWD", old_pwd);
                    set_env_var(global, "PWD", existing_parent);
                    return (0);
                }
            }
        }
    }
    printf("minishell: cd: %s: No such file or directory\n", path);
    return (1);
}

static char	*determine_cd_path(char **args, t_global *global)
{
    char	*home;

    if (!args[1])
    {
        home = get_env_value(global->env_list, "HOME");
        if (!home)
        {
            printf("minishell: cd: HOME not set\n");
            return (NULL);
        }
        return (home);
    }
    else
        return (args[1]);
}

int	builtin_cd(char **args, t_global *global)
{
    char	*path;
    char	*old_pwd;
    char    *cwd;
    old_pwd = get_env_value(global->env_list, "PWD");
    path = determine_cd_path(args, global);
    if (!path)
        return (1);
    if (args && args[1] && args[2])
    {
        printf("minishell: cd: too many arguments\n");
        return (1);
    }
    if (chdir(path) != 0)
        return (handle_chdir_failure(path, old_pwd, global));
    cwd = add_garbage(getcwd(NULL, 0));
    if (!cwd)
    {
        printf("cd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory\n");
        return (1);
    }
	update_pwd_variables(old_pwd, path, global);
    set_env_var(global, "OLDPWD", old_pwd);
    set_env_var(global, "PWD", cwd);
    return (0);
}

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
