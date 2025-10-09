/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:38:23 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:43:03 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <readline/history.h>
#include <readline/readline.h>

static int	process_input_utils(char **input, t_list **tokens)
{
	t_global	*global;
	int			len;

	global = get_global();
	if (!(*input) || ft_strlen((*input)) == 0)
		return (0);
	add_history((*input));
	(*input) = ft_strtrim((*input), " \t\n");
	if (!(*input)[0])
		return (0);
	len = ft_strlen((*input));
	if ((*input)[0] == '|' || (*input)[len - 1] == '|')
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		global->exit_status = 2;
		return (0);
	}
	global->input_line = ft_strdup((*input));
	(*tokens) = tokenize_advanced((*input), global);
	if (!(*tokens))
	{
		global->exit_status = 2;
		return (0);
	}
	return (1);
}

static int	process_input(char *input, t_global *global)
{
	t_list		*tokens;
	t_command	*commands;

	if (!process_input_utils(&input, &tokens))
		return (0);
	commands = parse_tokens_to_commands(tokens, global);
	if (!commands)
	{
		global->exit_status = 2;
		return (0);
	}
	global->tokens = tokens;
	global->commands = commands;
	if (commands)
		global->exit_status = execute_commands(commands, global);
	if (global->input_line)
		global->input_line = NULL;
	return (0);
}

static void	run_shell_loop(t_global *global)
{
	char	*input;
	int		should_exit;

	should_exit = 0;
	while (!should_exit && !global->should_exit)
	{
		input = readline(PROMPT);
		if (!input)
		{
			handle_eof();
			cleanup_and_exit();
			break ;
		}
		should_exit = process_input(input, global);
		free(input);
	}
}

t_global	*get_global(void)
{
	static t_global	minishell;

	return (&minishell);
}

int	main(int argc, char **argv, char **envp)
{
	t_global	*global;

	(void)argc;
	(void)argv;
	global = get_global();
	global = init_global(envp, global);
	if (!global)
	{
		printf("Error: Failed to initialize global state\n");
		cleanup_and_exit();
		return (1);
	}
	setup_signals();
	print_welcome_advanced();
	run_shell_loop(global);
	cleanup_and_exit();
	return (0);
}
