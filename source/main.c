/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 18:03:28 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/09 21:24:27 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

int main(int ac, char *av[], char **env)
{
	char	*input;
	
	(void)ac;
	(void)av;
	(void)env;
	while (1)
	{
		input = readline(PROMPT);
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (*input)
			add_history(input);
		
		printf("Girilen input: %s\n", input);
		free(input);
	}
	return (0);	
}