/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/31 23:26:03 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static void	skip_whitespace_advanced(t_lexer_new *lexer)
{
	while (lexer->current_char == ' ' || lexer->current_char == '\t'
		|| lexer->current_char == '\n')
		advance_lexer(lexer);
}

static t_token_new	*get_next_token(t_lexer_new *lexer)
{
	skip_whitespace_advanced(lexer);
	if (lexer->current_char == '\0')
		return (NULL);
	if (lexer->current_char == '|')
		return (handle_pipe_advanced(lexer));
	else if (lexer->current_char == '<' || lexer->current_char == '>')
		return (handle_redirect_advanced(lexer));
	else if (lexer->current_char == '\'' || lexer->current_char == '"')
		return (handle_quotes_advanced(lexer));
	else
		return (handle_word_advanced(lexer));
}

t_list	*tokenize_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;
	t_list		*tokens;
	t_token_new	*token;

	lexer = init_lexer_advanced(input, global);
	if (!lexer)
		return (NULL);
	tokens = NULL;
	while (lexer->current_char != '\0')
	{
		token = get_next_token(lexer);
		if (token)
			ft_lstadd_back(&tokens, ft_lstnew(token));
		else
			break ;
	}
	free_lexer_advanced(lexer);
	return (tokens);
}
