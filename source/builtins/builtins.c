/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:03 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/20 18:38:56 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

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

int	execute_builtin(t_command *cmd, t_global *global)
{
	int	original_stdout;
	int	original_stdin;
	int	result;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	
	// Redirection varsa file descriptor'ları kaydet ve setup et
	original_stdout = -1;
	original_stdin = -1;
	if (cmd->redirections)
	{
		original_stdout = dup(STDOUT_FILENO);
		original_stdin = dup(STDIN_FILENO);
		setup_redirections(cmd);
	}
	
	// Built-in komutunu çalıştır
	if (ft_strcmp(cmd->args[0], "pwd") == 0)
		result = builtin_pwd_global(global);
	else if (ft_strcmp(cmd->args[0], "echo") == 0)
		result = builtin_echo(cmd->args);
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		result = builtin_env(global->env_list);
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		result = builtin_exit(cmd->args, global);
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		result = builtin_cd(cmd->args, global);
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		result = builtin_export(cmd->args, global);
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		result = builtin_unset(cmd->args, global);
	else
		result = 1;
	
	// File descriptor'ları eski haline getir
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
	
	return (result);
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
	// free(cwd);
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
	if (!cwd) // mkdir -p a/b/c : rm -rf ../../b yapıp bir dizin geri gittiğinde girmesi gerek if
	{
		printf("pwd: error retrieving current directory: No such file or directory\n");
		return (1);
	}
	printf("%s\n", cwd);
	// free(cwd);
	return (0);
}

/* ************************************************************************** */
/*                            QUOTE PROCESSING HELPER                        */
/* ************************************************************************** */

char	*remove_quotes(char *str)
{
	int		len;
	int		i;
	int 	j;
	char	*result;
	//char	quote_char;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len < 2)
		return (ft_strdup(str));
	if ((str[0] == '"' && str[len - 1] == '"') ||
		(str[0] == '\'' && str[len - 1] == '\''))
	{
		// //quote_char = str[0];
		// result = ft_substr(str, 1, len - 2);
		// return (result);
		result = halloc(len - 1);  // halloc kullanıyor
        if (!result)
            return (NULL);
        i = 1;
        j = 0;
        while (i < len - 1)
        {
            result[j] = str[i];
            i++;
            j++;
        }
        result[j] = '\0';
        return (result);
	}
	return (ft_strdup(str));
}

/* ************************************************************************** */
/*                            ECHO BUILT-IN                                  */
/* ************************************************************************** */

static void	process_escape_sequences(char *str)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\\' && str[i + 1])
		{
			i++;
			if (str[i] == 'n')
				str[j++] = '\n';
			else if (str[i] == 't')
				str[j++] = '\t';
			else if (str[i] == 'r')
				str[j++] = '\r';
			else if (str[i] == '\\')
				str[j++] = '\\';
			else if (str[i] == '"')
				str[j++] = '"';
			else
			{
				str[j++] = '\\';
				str[j++] = str[i];
			}
		}
		else
			str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
}

int	builtin_echo(char **args)
{
	int		i;
	int		newline;
	int		enable_escape;
	char	*processed_arg;

	i = 1;
	newline = 1;
	enable_escape = 0;
	while (args[i] && args[i][0] == '-')
	{
		if (ft_strcmp(args[i], "-n") == 0)
			newline = 0;
		else if (ft_strcmp(args[i], "-e") == 0)
			enable_escape = 1;
		else if (ft_strcmp(args[i], "-E") == 0)
			enable_escape = 0;
		else
			break;
		i++;
	}
	while (args[i])
	{
		processed_arg = args[i];
		if (processed_arg)
		{
			if (enable_escape)
				process_escape_sequences(processed_arg);
			printf("%s", processed_arg);
			// free(processed_arg);
		}
		// if (args[i + 1])
		// 	printf(" ");
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
	// free_global(global);
	(void)*global;
	clear_garbage();
	exit(exit_code);
}
