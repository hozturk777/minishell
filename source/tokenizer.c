/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

void	add_token_to_list(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!new_token)
		return ;
	if (*tokens == NULL)
		*tokens = new_token;
	else
	{
		current = *tokens;
		while (current->next)
			current = current->next;
		current->next = new_token;
	}
}

static t_token	*get_next_token(t_lexer *lexer)
{
	if (lexer->current_char == '|')
		return (handle_pipe(lexer));
	else if (lexer->current_char == '>')
		return (handle_redirect_out(lexer));
	else if (lexer->current_char == '<')
		return (handle_redirect_in(lexer));
	else if (lexer->current_char == '"')
		return (handle_double_quote(lexer));
	else if (lexer->current_char == '\'')
		return (handle_single_quote(lexer));
	else
		return (handle_word(lexer));
}

t_token	*tokenize(char *input)
{
	t_lexer	*lexer;
	t_token	*tokens;
	t_token	*current_token;

	lexer = init_lexer(input);
	if (!lexer)
		return (NULL);
	tokens = NULL;
	while (lexer->current_char != '\0')
	{
		skip_whitespace(lexer);
		if (lexer->current_char == '\0')
			break ;
		current_token = get_next_token(lexer);
		if (current_token)
			add_token_to_list(&tokens, current_token);
	}
	free_lexer(lexer);
	return (tokens);
}

t_lexer	*init_lexer(char *input)
{
	t_lexer	*lexer;

	if (!input)
		return (NULL);
	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->pos = 0;
	lexer->len = ft_strlen(input);
	if (lexer->len > 0)
		lexer->current_char = input[0];
	else
		lexer->current_char = '\0';
	return (lexer);
}
