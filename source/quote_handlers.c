/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

t_token	*handle_double_quote(t_lexer *lexer)
{
	char	*value;
	int		start_pos;
	int		len;
	t_token	*token;

	advance(lexer);
	start_pos = lexer->pos;
	while (lexer->current_char != '"' && lexer->current_char != '\0')
		advance(lexer);
	len = lexer->pos - start_pos;
	value = ft_strndup(lexer->input + start_pos, len);
	if (lexer->current_char == '"')
		advance(lexer);
	token = create_token(TOKEN_DOUBLE_QUOTE, value);
	free(value);
	return (token);
}

t_token	*handle_single_quote(t_lexer *lexer)
{
	char	*value;
	int		start_pos;
	int		len;

	advance(lexer);
	start_pos = lexer->pos;
	while (lexer->current_char != '\'' && lexer->current_char != '\0')
		advance(lexer);
	len = lexer->pos - start_pos;
	value = ft_strndup(lexer->input + start_pos, len);
	if (lexer->current_char == '\'')
		advance(lexer);
	return (create_token(TOKEN_SINGLE_QUOTE, value));
}
