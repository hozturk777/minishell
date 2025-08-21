/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/22 02:36:17 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

t_token_new	*handle_pipe_advanced(t_lexer_new *lexer)
{
	advance_lexer(lexer);
	return (create_token_advanced(T_PIPE, "|"));
}

t_token_new	*handle_redirect_advanced(t_lexer_new *lexer)
{
	//int		start_pos;

	//start_pos = lexer->pos;
	if (lexer->current_char == '<')
	{
		advance_lexer(lexer);
		if (lexer->current_char == '<')
		{
			advance_lexer(lexer);
			return (create_token_advanced(T_HEREDOC, "<<"));
		}
		return (create_token_advanced(T_REDIRECT_IN, "<"));
	}
	else if (lexer->current_char == '>')
	{
		advance_lexer(lexer);
		if (lexer->current_char == '>')
		{
			advance_lexer(lexer);
			return (create_token_advanced(T_APPEND, ">>"));
		}
		return (create_token_advanced(T_REDIRECT_OUT, ">"));
	}
	return (NULL);
}

t_token_new	*handle_word_advanced(t_lexer_new *lexer)
{
	int		start;
	int		len;
	char	*word;

	start = lexer->pos;
	while (lexer->current_char != '\0' && lexer->current_char != ' '
		&& lexer->current_char != '\t' && lexer->current_char != '\n'
		&& lexer->current_char != '|' && lexer->current_char != '<'
		&& lexer->current_char != '>' && lexer->current_char != '\''
		&& lexer->current_char != '"')
		advance_lexer(lexer);
	len = lexer->pos - start;
	word = ft_substr(lexer->input, start, len);
	if (!word)
		return (NULL);
	return (create_token_advanced(T_WORD, word));
}

t_token_new	*handle_quotes_advanced(t_lexer_new *lexer)
{
	char	quote_char;
	int		start;
	int		len;
	char	*quoted_str;

	quote_char = lexer->current_char;
	advance_lexer(lexer);
	start = lexer->pos;
	while (lexer->current_char != '\0' && lexer->current_char != quote_char)
		advance_lexer(lexer);
	if (lexer->current_char == quote_char)
	{
		len = lexer->pos - start;
		quoted_str = ft_substr(lexer->input, start, len);
		advance_lexer(lexer);
		if (quote_char == '\'')
			return (create_token_advanced(T_SINGLE_QUOTE, quoted_str));
		else
			return (create_token_advanced(T_DOUBLE_QUOTE, quoted_str));
	}
	return (NULL);
}
