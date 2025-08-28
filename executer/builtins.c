/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:03 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/28 17:39:07 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

/* ************************************************************************** */
/*                            BUILT-IN KONTROL FONKSIYONU                    */
/* ************************************************************************** */

int	is_builtin(char *command) // Define olarak ya da enum olarak eklenecek her biri
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

/* ************************************************************************** */
/*                            BUILT-IN EXECUTOR                              */
/* ************************************************************************** */

int	execute_builtin(t_command *cmd, t_global *global) // Define olarak ya da enum olarak eklenecek her biri
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd_global(global));
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args));
	if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(global->env_list));
	if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (builtin_exit(cmd->args, global));
	if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (builtin_cd(cmd->args, global));
	if (ft_strcmp(cmd->args[0], "export") == 0)
		return (builtin_export(cmd->args, global));
	if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (builtin_unset(cmd->args, global));
	return (1);
}

/* ************************************************************************** */
/*                            PWD BUILT-IN                                   */
/* ************************************************************************** */

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		printf("pwd: error retrieving current directory: No such file or directory\n");
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}

/* PWD built-in with global support */
int	builtin_pwd_global(t_global *global)
{
	char	*pwd_env;
	char	*cwd;

	pwd_env = get_env_value(global->env_list, "PWD");
	if (pwd_env)
	{
		printf("%s\n", pwd_env);
		return (0);
	}
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		printf("pwd: error retrieving current directory: No such file or directory\n");
		return (1);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (0);
}

/* ************************************************************************** */
/*                            QUOTE PROCESSING HELPER                        */
/* ************************************************************************** */

char	*remove_quotes(char *str)
{
	int		len;
	char	*result;
	char	quote_char;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len < 2)
		return (ft_strdup(str));
	if ((str[0] == '"' && str[len - 1] == '"') ||
		(str[0] == '\'' && str[len - 1] == '\''))
	{
		quote_char = str[0];
		result = ft_substr(str, 1, len - 2);
		return (result);
	}
	return (ft_strdup(str));
}

/* ************************************************************************** */
/*                            ECHO BUILT-IN                                  */
/* ************************************************************************** */

int	builtin_echo(char **args)
{
	int		i;
	int		newline;
	char	*processed_arg;

	i = 1;
	newline = 1;
	if (args[1] && ft_strcmp(args[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	while (args[i])
	{
		processed_arg = remove_quotes(args[i]);
		if (processed_arg)
		{
			printf("%s", processed_arg);
			free(processed_arg);
		}
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

/* ************************************************************************** */
/*                            ENV BUILT-IN                                   */
/* ************************************************************************** */

int	builtin_env(t_env *env_list)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (current->value)
			printf("%s=%s\n", current->key, current->value);
		current = current->next;
	}
	return (0);
}

/* ************************************************************************** */
/*                            EXIT BUILT-IN                                  */
/* ************************************************************************** */

int	builtin_exit(char **args, t_global *global)
{
	int	exit_code;

	exit_code = 0;
	printf("exit\n");
	if (args[1])
	{
		exit_code = ft_atoi(args[1]);
		if (args[2])
		{
			printf("minishell: exit: too many arguments\n");
			return (1);
		}
	}
	free_global(global);
	exit(exit_code);
}
