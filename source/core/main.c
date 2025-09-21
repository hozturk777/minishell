/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:38:23 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/21 21:45:10 by hsyn             ###   ########.fr       */
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
	input = ft_strtrim(input, " "); // Düzenlenecek whitespace eklenecek & NULL check eklenecek
	global->input_line = ft_strdup(input);
	
	// 1. LEXER: Input'u token'lara çevir
	tokens = tokenize_advanced(input, global);
	if (!tokens)
	{
		printf("Error: Tokenization failed\n");
		return (0);
	}
	
	// 2. PARSER: Token'ları komut yapılarına çevir
	commands = parse_tokens_to_commands(tokens, global);
	if (!commands)
	{
		printf("Error: Parsing failed\n");
		//free_tokens_advanced(&tokens);
		if (global->input_line)
            free(global->input_line);
        global->input_line = NULL;
		return (0);
	}
	// 3. DEBUG: Token'ları ve komutları yazdır
	// printf("\n=== TOKENS ===\n");
	// print_tokens_advanced(tokens);
	// printf("\n=== PARSED COMMANDS ===\n");
	// print_commands_debug(commands);
	// printf("\n==================\n\n");
	
	// 4. EXECUTION: Komutları çalıştır
	if (commands)
	{
		global->exit_status = execute_commands(commands, global);
		debug_print("Commands executed successfully");
	}
	
	// 5. CLEANUP: Memory'yi temizle
	global->tokens = tokens;
	global->commands = commands;
	//free_tokens_advanced(&global->tokens);
	//free_commands_list(global->commands);
	global->commands = NULL;
	
	if (global->input_line)
	{
		// free(global->input_line);
		global->input_line = NULL;
	}
	
	if (ft_strncmp(input, "exit", 4) == 0)
	{
		clear_garbage();
		return (1);
	}
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
			//clear_garbage();
			break ;
		}
		should_exit = process_input(input, global);
		free(input);
	}
	//clear_garbage();
}

int	main(int argc, char **argv, char **envp) // argc sayısı check
{
	t_global	*global;

	(void)argc;
	(void)argv;
	global = init_global(envp);
	if (!global)
	{
		printf("Error: Failed to initialize global state\n");
		clear_garbage();
		return (1);
	}
	
	// Sinyalleri ayarla (ARAŞTIRILACAK)
	setup_signals();
	
	print_welcome_advanced();
	debug_print("Global state initialized successfully");
	debug_print("Signal handlers set up successfully");
	
	run_shell_loop(global);
	
	printf(GREEN "Goodbye!" RESET "\n");
	// free_global(global);
	clear_garbage();
	return (0);
}
