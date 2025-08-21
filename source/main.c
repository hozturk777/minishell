/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_advanced.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/01/08 12:00:00 by huozturk         ###   ########.fr       */
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
	t_list	*tokens;

	if (!input || ft_strlen(input) == 0)
		return (0);
	add_history(input);
	global->input_line = ft_strdup(input);
	tokens = tokenize_advanced(input, global);
	if (tokens)
	{
		global->tokens = tokens;
		print_tokens_advanced(tokens);
		free_tokens_advanced(&global->tokens);
	}
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
