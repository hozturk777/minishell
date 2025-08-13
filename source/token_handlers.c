/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

t_token	*handle_pipe(t_lexer *lexer)
{
	t_token	*token;

	token = create_token(TOKEN_PIPE, "|");
	advance(lexer);
	return (token);
}

t_token	*handle_redirect_out(t_lexer *lexer)
{
	t_token	*token;

	advance(lexer);
	if (lexer->current_char == '>')
	{
		token = create_token(TOKEN_APPEND, ">>");
		advance(lexer);
	}
	else
		token = create_token(TOKEN_REDIRECT_OUT, ">");
	return (token);
}

t_token	*handle_redirect_in(t_lexer *lexer)
{
	t_token	*token;

	advance(lexer);
	if (lexer->current_char == '<')
	{
		token = create_token(TOKEN_HEREDOC, "<<");
		advance(lexer);
	}
	else
		token = create_token(TOKEN_REDIRECT_IN, "<");
	return (token);
}

t_token	*handle_word(t_lexer *lexer)
{
	char	*value;
	int		start_pos;
	int		len;
	t_token	*token;

	start_pos = lexer->pos;
	while (lexer->current_char != '\0' && lexer->current_char != ' '
		&& lexer->current_char != '\t' && lexer->current_char != '|'
		&& lexer->current_char != '>' && lexer->current_char != '<'
		&& lexer->current_char != '"' && lexer->current_char != '\'')
		advance(lexer);
	len = lexer->pos - start_pos;
	value = ft_strndup(lexer->input + start_pos, len);
	token = create_token(TOKEN_WORD, value);
	free(value);
	return (token);
}
