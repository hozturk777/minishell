/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

int	main(int ac, char *av[], char **env)
{
	char	*input;
	t_token	*tokens;

	(void)ac;
	(void)av;
	(void)env;
	print_welcome();
	printf("\n=== LEXER TEST MODE ===\n");
	printf("Test komutları:\n");
	printf("- echo hello\n");
	printf("- ls | grep main\n");
	printf("- echo \"hello world\"\n");
	printf("- cat > file.txt\n");
	printf("- exit (çıkmak için)\n\n");
	while (1)
	{
		input = readline(GREEN PROMPT RESET);
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (*input)
		{
			add_history(input);
			tokens = tokenize(input);
			print_tokens(tokens);
			free_tokens(tokens);
		}
		if (ft_strncmp(input, "exit", 4) == 0 && ft_strlen(input) == 4)
		{
			free(input);
			break ;
		}
		free(input);
	}
	return (0);
}
