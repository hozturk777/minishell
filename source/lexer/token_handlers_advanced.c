/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handle_advanced.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 09:12:36 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/29 09:13:12 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static t_token_new	*handle_input_redirect(t_lexer_new *lexer)
{
	advance_lexer(lexer);
	lexer->t_cmd_flag = 2;
	if (lexer->current_char == '<')
	{
		advance_lexer(lexer);
		skip_whitespace_advanced(lexer);
		return (create_token_advanced(T_HEREDOC, "<<"));
	}
	return (create_token_advanced(T_REDIRECT_IN, "<"));
}

static t_token_new	*handle_output_redirect(t_lexer_new *lexer)
{
	advance_lexer(lexer);
	lexer->t_cmd_flag = 2;
	if (lexer->current_char == '>')
	{
		advance_lexer(lexer);
		skip_whitespace_advanced(lexer);
		return (create_token_advanced(T_APPEND, ">>"));
	}
	return (create_token_advanced(T_REDIRECT_OUT, ">"));
}

t_token_new	*handle_redirect_advanced(t_lexer_new *lexer)
{
	if (lexer->current_char == '<')
		return (handle_input_redirect(lexer));
	else if (lexer->current_char == '>')
		return (handle_output_redirect(lexer));
	return (NULL);
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
		if (len == 0)
			return (create_token_advanced(T_SINGLE_QUOTE, ft_strdup("")));
		if (quote_char == '\'')
			return (create_token_advanced(T_SINGLE_QUOTE, quoted_str));
		else
			return (create_token_advanced(T_DOUBLE_QUOTE, quoted_str));
	}
	return (NULL);
}

t_token_new	*handle_whitespaces_advanced(t_lexer_new *lexer)
{
	int		start;
	int		len;
	char	*whitepaces;

	start = lexer->pos;
	while (lexer->current_char != '\0'
		&& (lexer->current_char == ' '
			|| lexer->current_char == '\t'))
	{
		advance_lexer(lexer);
	}
	len = lexer->pos - start;
	whitepaces = " ";
	return (create_token_advanced(T_WHITESPACE, whitepaces));
}
