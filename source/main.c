/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 18:03:28 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/11 17:25:33 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

void print_welcome(void)
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
    printf("║                                    \033[1;33mCreated by: huozturk\033[1;36m                                   ║\n");
	usleep(20000);
    printf("║                                     \033[1;32m42 School Project\033[1;36m                                     ║\n");
	usleep(20000);
    printf("║                                                                                           ║\n");
	usleep(20000);
    printf("║                            \033[1;31m🚀 Welcome to your custom shell! 🚀\033[1;36m                            ║\n");
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

int main(int ac, char *av[], char **env)
{
	char	*input;
	
	(void)ac;
	(void)av;
	(void)env;
	while (1)
	{
		print_welcome();
		input = readline(GREEN PROMPT RESET);
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (*input)
			add_history(input);
		
		free(input);
	}
	return (0);	
}