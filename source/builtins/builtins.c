/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abakirca <abakirca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:03 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/01 18:47:18 by abakirca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>

int	is_builtin(char *command)
{
	if (!command)
		return (0);
	if (ft_strcmp(command, "pwd") == 0)
		return (1);
	if (ft_strcmp(command, "echo") == 0)
		return (1);
	if (ft_strcmp(command, "env") == 0)
		return (1);
	if (ft_strcmp(command, "exit") == 0)
		return (1);
	if (ft_strcmp(command, "cd") == 0)
		return (1);
	if (ft_strcmp(command, "export") == 0)
		return (1);
	if (ft_strcmp(command, "unset") == 0)
		return (1);
	return (0);
}

static void	restore_file_descriptors(t_command *cmd, int original_stdout, int original_stdin)
{
    if (cmd->redirections)
    {
        if (original_stdout != -1)
        {
            dup2(original_stdout, STDOUT_FILENO);
            close(original_stdout);
        }
        if (original_stdin != -1)
		{
            dup2(original_stdin, STDIN_FILENO);
            close(original_stdin);
		}
	}
	else
	{
		close(original_stdin);
		close(original_stdout);
	}
}

static int	execute_builtin_command(t_command *cmd, t_global *global)
{
    if (ft_strcmp(cmd->args[0], "pwd") == 0)
        return (builtin_pwd_global(global));
    else if (ft_strcmp(cmd->args[0], "echo") == 0)
        return (builtin_echo(cmd->args));
    else if (ft_strcmp(cmd->args[0], "env") == 0)
        return (builtin_env(global->env_list));
    else if (ft_strcmp(cmd->args[0], "exit") == 0)
        return (builtin_exit(cmd->args));
    else if (ft_strcmp(cmd->args[0], "cd") == 0)
        return (builtin_cd(cmd->args, global));
    else if (ft_strcmp(cmd->args[0], "export") == 0)
        return (builtin_export(cmd->args, global));
    else if (ft_strcmp(cmd->args[0], "unset") == 0)
        return (builtin_unset(cmd->args, global));
    else
        return (1);
}


int	execute_builtin(t_command *cmd, t_global *global, int *originals, int flag)
{

    int	result;

    if (!cmd || !cmd->args || !cmd->args[0])
        return (1);

    if (cmd->redirections)
    {
		if (flag)
			global->in_child = 2;
		execute_redirect_command(cmd, global, originals);
		global->in_child = 0;
        // if(setup_redirections(cmd))
		// {
		// 	close(originals[1]);
		// 	close(originals[0]);
		// 	return (1);
		// }
		// close(originals[1]);
		// close(originals[0]);
    }
    result = execute_builtin_command(cmd, global);
    restore_file_descriptors(cmd, originals[1], originals[0]);
    return (result);
}

int	builtin_pwd_global(t_global *global)
{
	t_command	*cmd;
	char		*pwd_env;
	char		*cwd;

	cmd = global->commands;
	while (cmd)
	{
		if (cmd->args && !ft_strcmp(cmd->args[0], "pwd")  && cmd->args[1])
		{
			if (cmd->args[1][0] && cmd->args[1][0] == '-' && cmd->args[1][1])
			{
				printf("minishell: pwd: -%c: invalid option\n", cmd->args[1][1]);
				return (2);
			}
		}
		cmd = cmd->next;
	}
	pwd_env = get_env_value(global->env_list, "PWD");
	if (pwd_env)
	{
		printf("%s\n", pwd_env);
		return (0);
	}
	cwd = add_garbage(getcwd(NULL, 0));
	if (!cwd)
	{
		printf("pwd: error retrieving current directory: No such file or directory\n");
		return (1);
	}
	printf("%s\n", cwd);
	return (0);
}
