/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 18:03:28 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/11 18:44:30 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include "../lib/minishell.h"

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


t_token	*tokenize(char *input);
t_lexer	*init_lexer(char *input);


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

t_token	*tokenize(char *input)
{
	t_lexer	*lexer = init_lexer(input); // BURADA NE YAPILACAK ?
}

t_lexer	*init_lexer(char *input)
{
	t_lexer *lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)	// ERROR_HANDLE YAZILACAK
		return (NULL);
	lexer->input = input;
	lexer->input_current_char = input[0];
	lexer->input_len = ft_strlen(input); // LIBFT EKLENECEK
	lexer->input_pos = 0;
	return (lexer);
}
