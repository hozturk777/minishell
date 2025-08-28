/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:38:23 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/28 17:38:27 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

static void	print_welcome_advanced(void)
{
	printf("\033[2J\033[H"); // Ekranı temizle
    printf("\033[1;36m"); // Cyan bold
    printf("╔═══════════════════════════════════════════════════════════════════════════════════════════╗\n");
	usleep(20000);
    printf("║                                                                                           ║\n");
	usleep(20000);
    printf("║             \033[1;35m███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║     ██║\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║     ██║\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗\033[1;36m           ║\n");
	usleep(20000);
    printf("║             \033[1;35m╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\033[1;36m           ║\n");
	usleep(20000);
    printf("║                                                                                           ║\n");
	usleep(20000);
    printf("║                             \033[1;33mCreated by: hasivaci && huozturk\033[1;36m                              ║\n");
	usleep(20000);
    printf("║                                     \033[1;32m42 School Project\033[1;36m                                     ║\n");
	usleep(20000);
    printf("║                                                                                           ║\n");
	usleep(20000);
    printf("║                                   \033[1;31m🚀 SelamunAleyküm! 🚀\033[1;36m                                   ║\n");
	usleep(20000);
    printf("║                                                                                           ║\n");
	usleep(20000);
    printf("╚═══════════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n");
	usleep(20000);
    printf("\n");
	usleep(20000);
    printf("\033[1;32mType 'exit' to quit the shell.\033[0m\n");
	usleep(20000);
    printf("\033[1;34mEnjoy coding! 💻\033[0m\n\n");
	usleep(20000);
}

static int	process_input(char *input, t_global *global)
{
	t_list		*tokens;
	t_command	*commands;

	if (!input || ft_strlen(input) == 0)
		return (0);
	add_history(input);
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
		free_tokens_advanced(&tokens);
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
	free_tokens_advanced(&global->tokens);
	free_commands_list(global->commands);
	global->commands = NULL;
	
	if (global->input_line)
	{
		free(global->input_line);
		global->input_line = NULL;
	}
	
	if (ft_strncmp(input, "exit", 4) == 0)
		return (1);
	return (0);
}

static void	run_shell_loop(t_global *global)
{
	char	*input;
	int		should_exit;

	should_exit = 0;
	while (!should_exit)
	{
		input = readline(PROMPT);
		if (!input)
		{
			printf("\n");
			break ;
		}
		should_exit = process_input(input, global);
		free(input);
	}
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
		return (1);
	}
	print_welcome_advanced();
	debug_print("Global state initialized successfully");
	run_shell_loop(global);
	printf(GREEN "Goodbye!" RESET "\n");
	free_global(global);
	return (0);
}
