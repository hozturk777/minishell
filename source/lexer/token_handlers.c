/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/30 19:49:58 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

t_token_new	*handle_pipe_advanced(t_lexer_new *lexer)
{
	advance_lexer(lexer);
	return (create_token_advanced(T_PIPE, "|"));
}

static t_token_new	*extract_regular_word(t_lexer_new *lexer, int start, t_token_types types)
{
	int		len;
	char	*word;

	while (lexer->current_char != '\0' && lexer->current_char != ' '
		&& lexer->current_char != '\t' && lexer->current_char != '\n'
		&& lexer->current_char != '|' && lexer->current_char != '<'
		&& lexer->current_char != '>' && lexer->current_char != '\''
		&& lexer->current_char != '"')
		advance_lexer(lexer);
	len = lexer->pos - start;
	if (len > 0)
	{
		word = ft_substr(lexer->input, start, len);
	}
	else
		return (NULL);
	if (!word)
		return (NULL);
	return (create_token_advanced(types, word));
}

static t_token_new	*process_word_content(t_lexer_new *lexer, int start, t_token_types types)
{
    int		len;
    char	*word;

    if (lexer->current_char == '$' && lexer->input[lexer->pos + 1] == '"')
    {
        advance_lexer(lexer);
        advance_lexer(lexer);
        while (lexer->current_char != '\0' && lexer->current_char != '"')
            advance_lexer(lexer);
        if (lexer->current_char == '"')
            advance_lexer(lexer);
        len = lexer->pos - start;
        word = ft_substr(lexer->input, start + 2, len - 3);
        if (!word)
            return (NULL);
        return (create_token_advanced(types, word));
    }
    return (extract_regular_word(lexer, start, types));
}

t_token_new	*handle_word_advanced(t_lexer_new *lexer)
{
    int				start;
    t_token_types	types;

    start = lexer->pos;
    types = T_WORD;
    if (!lexer || !lexer->input || lexer->pos >= lexer->len)
        return (NULL);
    if (lexer->pos == 0 || lexer->t_cmd_flag == 1)
    {
        types = T_CMD;
        lexer->t_cmd_flag = 0;
    }
    else
    {
        types = T_WORD;
		if (lexer->t_cmd_flag == 2)
			lexer->t_cmd_flag = 1;
    }
    return (process_word_content(lexer, start, types));
}
