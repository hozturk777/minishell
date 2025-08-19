/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_advanced.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/01/08 12:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

t_lexer_new	*init_lexer_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;

	lexer = (t_lexer_new *)malloc(sizeof(t_lexer_new));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->pos = 0;
	lexer->len = ft_strlen(input);
	lexer->global = global;
	if (lexer->len > 0)
		lexer->current_char = input[0];
	else
		lexer->current_char = '\0';
	return (lexer);
}

void	free_lexer_advanced(t_lexer_new *lexer)
{
	if (lexer)
		free(lexer);
}

void	advance_lexer(t_lexer_new *lexer)
{
	lexer->pos++;
	if (lexer->pos >= lexer->len)
		lexer->current_char = '\0';
	else
		lexer->current_char = lexer->input[lexer->pos];
}

t_token_new	*create_token_advanced(t_token_types type, char *value)
{
	t_token_new	*token;

	token = (t_token_new *)malloc(sizeof(t_token_new));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->len = ft_strlen(value);
	token->quote_type = 0;
	token->expanded = 0;
	return (token);
}
