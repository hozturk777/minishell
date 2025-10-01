/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:03 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:29:08 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

/* ************************************************************************** */
/*                            BUILT-IN KONTROL FONKSIYONU                    */
/* ************************************************************************** */
// Define olarak ya da enum olarak eklenecek her biri
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

/* ************************************************************************** */
/*                            BUILT-IN EXECUTOR                              */
/* ************************************************************************** */


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


int	execute_builtin(t_command *cmd, t_global *global)
{
    int	original_stdout;
    int	original_stdin;
    int	result;

    if (!cmd || !cmd->args || !cmd->args[0])
        return (1);
    original_stdout = -1;
    original_stdin = -1;
    if (cmd->redirections)
    {
        original_stdout = dup(STDOUT_FILENO);
        original_stdin = dup(STDIN_FILENO);
        setup_redirections(cmd);
    }
    result = execute_builtin_command(cmd, global);
    restore_file_descriptors(cmd, original_stdout, original_stdin);
    return (result);
}



// int	execute_builtin(t_command *cmd, t_global *global)
// {
// 	int	original_stdout;
// 	int	original_stdin;
// 	int	result;

// 	if (!cmd || !cmd->args || !cmd->args[0])
// 		return (1);
	
// 	// Redirection varsa file descriptor'ları kaydet ve setup et
// 	original_stdout = -1;
// 	original_stdin = -1;
// 	if (cmd->redirections)
// 	{
// 		original_stdout = dup(STDOUT_FILENO);
// 		original_stdin = dup(STDIN_FILENO);
// 		setup_redirections(cmd);
// 	}
// 	if (ft_strcmp(cmd->args[0], "pwd") == 0)
// 		result = builtin_pwd_global(global);
// 	else if (ft_strcmp(cmd->args[0], "echo") == 0)
// 		result = builtin_echo(cmd->args);
// 	else if (ft_strcmp(cmd->args[0], "env") == 0)
// 		result = builtin_env(global->env_list);
// 	else if (ft_strcmp(cmd->args[0], "exit") == 0)
// 		result = builtin_exit(cmd->args);
// 	else if (ft_strcmp(cmd->args[0], "cd") == 0)
// 		result = builtin_cd(cmd->args, global);
// 	else if (ft_strcmp(cmd->args[0], "export") == 0)
// 		result = builtin_export(cmd->args, global);
// 	else if (ft_strcmp(cmd->args[0], "unset") == 0)
// 		result = builtin_unset(cmd->args, global);
// 	else
// 		result = 1;
	
// 	// File descriptor'ları eski haline getir
// 	if (cmd->redirections)
// 	{
// 		if (original_stdout != -1)
// 		{
// 			dup2(original_stdout, STDOUT_FILENO);
// 			close(original_stdout);
// 		}
// 		if (original_stdin != -1)
// 		{
// 			dup2(original_stdin, STDIN_FILENO);
// 			close(original_stdin);
// 		}
// 	}
	
// 	return (result);
// }

/* ************************************************************************** */
/*                            PWD BUILT-IN                                   */
/* ************************************************************************** */

// int	builtin_pwd(void)
// {
// 	char	*cwd;

// 	cwd = getcwd(NULL, 0);
// 	if (!cwd)
// 	{
// 		printf("pwd: error retrieving current directory: No such file or directory\n");
// 		return (1);
// 	}
// 	printf("%s\n", cwd);
// 	// free(cwd);
// 	return (0);
// }

/* PWD built-in with global support */
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
				//bash: pwd: -d: invalid option
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
	cwd = getcwd(NULL, 0);
	if (!cwd) // mkdir -p a/b/c : rm -rf ../../b yapıp bir dizin geri gittiğinde girmesi gerek if
	{
		printf("pwd: error retrieving current directory: No such file or directory\n");
		return (1);
	}
	printf("%s\n", cwd);
	return (0);
}

// /* ************************************************************************** */
// /*                            QUOTE PROCESSING HELPER                        */
// /* ************************************************************************** */

// char	*remove_quotes(char *str)
// {
// 	int		len;
// 	int		i;
// 	int 	j;
// 	char	*result;
// 	//char	quote_char;

// 	if (!str)
// 		return (NULL);
// 	len = ft_strlen(str);
// 	if (len < 2)
// 		return (ft_strdup(str));
// 	if ((str[0] == '"' && str[len - 1] == '"') ||
// 		(str[0] == '\'' && str[len - 1] == '\''))
// 	{
// 		// //quote_char = str[0];
// 		// result = ft_substr(str, 1, len - 2);
// 		// return (result);
// 		result = halloc(len - 1);  // halloc kullanıyor
//         if (!result)
//             return (NULL);
//         i = 1;
//         j = 0;
//         while (i < len - 1)
//         {
//             result[j] = str[i];
//             i++;
//             j++;
//         }
//         result[j] = '\0';
//         return (result);
// 	}
// 	return (ft_strdup(str));
// }

/* ************************************************************************** */
/*                            ECHO BUILT-IN                                  */
/* ************************************************************************** */

// static void	process_escape_sequences(char *str)
// {
// 	int	i;
// 	int	j;

// 	i = 0;
// 	j = 0;
// 	while (str[i])
// 	{
// 		if (str[i] == '\\' && str[i + 1])
// 		{
// 			if (str[i + 1] == '\\')
// 			{
// 				str[i] = '\\';
// 				str[j] = str[i];

// 			}
// 			else
// 			{
// 				str[j] = str[i+1];
// 			}
// 			i++;
// 			// j++;
// 		}		
// 		else
// 			str[j] = str[i];
// 		j++;
// 		i++;
// 	}
// 	str[j] = '\0';
// }

// int	builtin_echo(char **args)
// {
// 	int		i;
// 	int		j;
// 	int		b;
// 	int		next_g;
// 	int		newline;
// 	int		n_flag;
// 	int		enable_escape;
// 	char	*processed_arg;

// 	i = 1;
// 	j = 1;
// 	b = 0;
// 	n_flag = 0;
// 	next_g = 0;
// 	newline = 1;
// 	enable_escape = 0;
	
// 	if (args[1] && args[1][next_g] == '-')
// 	{
// 		next_g++;
// 		while (args[1][next_g])
// 		{
// 			if (args[1][next_g] == 'n')
// 				n_flag = 1;
// 			else
// 			{
// 				n_flag = 0;
// 				break;
// 			}
// 			next_g++;
// 		}

// 		if (n_flag)
// 		{
// 			newline = 0;
// 			i++;
// 			if (args[i][0] != ' ')
// 			{
// 				newline = 1;
// 				i--;
// 			}
			
// 			while (!ft_strcmp(args[i], " ") || !ft_strcmp(args[i], "\t"))
// 			{
// 				i++;
// 			}
			
// 		}
// 	}
// 	j = i;
// 	while (args[j])
// 	{
// 		while (args[j][b])
// 		{
// 			if (args[j][b] == '\\')
// 				enable_escape = 1;
// 			b++;
// 		}
		
// 		j++;
// 	}
	
// 	j = i;
// 	while (args[j])
// 	{
// 		processed_arg = args[j];
// 		if (processed_arg)
// 		{
// 			if (enable_escape)
// 			{
// 				process_escape_sequences(processed_arg);
// 				enable_escape = 0;
// 			}
// 			printf("%s", processed_arg);
// 		}
// 		j++;
// 	}
// 	if (newline)
// 		printf("\n");
// 	return (0);
// }

// /* ************************************************************************** */
// /*                            ENV BUILT-IN                                   */
// /* ************************************************************************** */

// int	builtin_env(t_env *env_list)
// {
// 	t_env		*current;
// 	t_global	*global;

// 	current = env_list;
// 	global = get_global();
// 	if (global->commands->args[1] && global->commands->args[1][0] == '-')
// 	{
// 		printf("env: invalid option -- '%c'\n", global->commands->args[1][1]);
// 		return (125);
// 	}
// 	else if (global->commands->args[1])
// 	{
// 		printf("env: '%s': No such file or directory\n", global->commands->args[1]);	
// 		return (127);
// 		//	env: ‘dsadsa’: No such file or directory
	
// 	}
	
	
// 	while (current)
// 	{
// 		if (current->value)
// 			printf("%s=%s\n", current->key, current->value);
// 		current = current->next;
// 	}
// 	return (0);
// }

// /* ************************************************************************** */
// /*                            EXIT BUILT-IN                                  */
// /* ************************************************************************** */

// int	builtin_exit(char **args)
// {
// 	int	exit_code;

// 	exit_code = 0;
// 	printf("exit\n");
// 	int i;
// 	i = 0;
// 	if (args[1])
// 	{
// 		// printf("exit code : %d\n", exit_code);
// 		while(args[1][i])
// 		{
// 			while (args[1][i] == '+' || args[1][i] == '-')
// 				i++;
// 			if (!ft_isdigit(args[1][i]))
// 			{
// 				printf("minishell: exit: %s: numeric argument required\n", args[1]);
// 				cleanup_and_exit();  // FD'leri kapat ve garbage collect
// 				exit(2);
// 			}
// 			i++;
// 		}
// 		exit_code = ft_atoi(args[1]);
// 		if (args[2])
// 		{
// 			printf("minishell: exit: too many arguments\n");
// 			return (0);
// 		}
// 	}
// 	// free_global(global);
// 	// (void)*global;
// 	cleanup_and_exit();  // FD'leri kapat ve garbage collect
// 	exit(exit_code);
// }
