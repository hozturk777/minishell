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

static int	process_input(char *input, t_global *global)
{
	t_list		*tokens;
	t_command	*commands;

	if (!input || ft_strlen(input) == 0)
		return (0);
	add_history(input);
	input = ft_strtrim(input, " ");
		// Düzenlenecek whitespace eklenecek & NULL check eklenecek
	global->input_line = ft_strdup(input);
	// 1. LEXER: Input'u token'lara çevir
	tokens = tokenize_advanced(input, global);
	if (!tokens)
	{
		global->exit_status = 2;
		return (0);
	}
	// printf("\n=== TOKENS ===\n");
	// print_tokens_advanced(tokens);
	// 2. PARSER: Token'ları komut yapılarına çevir
	commands = parse_tokens_to_commands(tokens, global); // NULL DÖNME İHTİMALİ VAR!!
	// if (!commands) 
	// {
	// 	printf("Error: Parsing failed\n");
	// 	//free_tokens_advanced(&tokens);
	// 	if (global->input_line)
	//         free(global->input_line);
	//     global->input_line = NULL;
	// 	return (0);
	// }
	// 3. DEBUG: Token'ları ve komutları yazdır
	// printf("\n=== TOKENS ===\n");
	// print_tokens_advanced(tokens);
	// printf("\n=== PARSED COMMANDS ===\n");
	// print_commands_debug(commands);
	// printf("\n==================\n\n");
	global->tokens = tokens;
	global->commands = commands;
	// 4. EXECUTION: Komutları çalıştır
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
			// EOF (Ctrl+D) algılandı
			handle_eof();
			// rl_clear_history();
			cleanup_and_exit(); // FD'leri kapat ve garbage collect
			break ;
		}
		should_exit = process_input(input, global);
		free(input);
	}
	// clear_garbage();
}

t_global	*get_global(void)
{
	static t_global	minishell;

	return (&minishell);
}

// argc sayısı check
int	main(int argc, char **argv, char **envp)
{
	printf("\nDEBUG: Main process PID: %d\n", getpid());

	t_global	*global;

	(void)argc;
	(void)argv;
	global = get_global();
	global = init_global(envp, global);
	if (!global)
	{
		printf("Error: Failed to initialize global state\n");
		cleanup_and_exit(); // FD'leri kapat ve garbage collect
		return (1);
	}
	// Exit cleanup handler kaydet
	// atexit(cleanup_and_exit);
	// Sinyalleri ayarla (ARAŞTIRILACAK)
	setup_signals();
	print_welcome_advanced();
	debug_print("Global state initialized successfully");
	debug_print("Signal handlers set up successfully");
	run_shell_loop(global);
	printf(GREEN "Goodbye!" RESET "\n");
	cleanup_and_exit(); // FD'leri kapat ve garbage collect
	return (0);
}
