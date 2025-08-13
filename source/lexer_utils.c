/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

char	*ft_strndup(const char *s, size_t n)
{
	char	*result;
	size_t	i;

	if (!s)
		return (NULL);
	result = malloc(n + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (i < n && s[i])
	{
		result[i] = s[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	if (value)
		token->len = ft_strlen(value);
	else
		token->len = 0;
	token->next = NULL;
	return (token);
}

void	advance(t_lexer *lexer)
{
	lexer->pos++;
	if (lexer->pos >= lexer->len)
		lexer->current_char = '\0';
	else
		lexer->current_char = lexer->input[lexer->pos];
}

void	skip_whitespace(t_lexer *lexer)
{
	while (lexer->current_char == ' ' || lexer->current_char == '\t'
		|| lexer->current_char == '\n')
		advance(lexer);
}

void	free_lexer(t_lexer *lexer)
{
	if (lexer)
		free(lexer);
}
