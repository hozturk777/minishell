/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

static void	print_token_type(t_token_type type)
{
	if (type == TOKEN_WORD)
		printf("WORD");
	else if (type == TOKEN_PIPE)
		printf("PIPE");
	else if (type == TOKEN_REDIRECT_IN)
		printf("REDIRECT_IN");
	else if (type == TOKEN_REDIRECT_OUT)
		printf("REDIRECT_OUT");
	else if (type == TOKEN_APPEND)
		printf("APPEND");
	else if (type == TOKEN_HEREDOC)
		printf("HEREDOC");
	else if (type == TOKEN_SINGLE_QUOTE)
		printf("SINGLE_QUOTE");
	else if (type == TOKEN_DOUBLE_QUOTE)
		printf("DOUBLE_QUOTE");
	else if (type == TOKEN_ENV_VAR)
		printf("ENV_VAR");
	else
		printf("UNKNOWN");
}

void	print_tokens(t_token *tokens)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	printf("\n=== TOKEN LIST ===\n");
	if (!tokens)
	{
		printf("No tokens found.\n");
		printf("==================\n\n");
		return ;
	}
	while (current)
	{
		printf("Token %d: [%s] Type: %d (", i, current->value, current->type);
		print_token_type(current->type);
		printf(")\n");
		current = current->next;
		i++;
	}
	printf("==================\n\n");
}

void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}
