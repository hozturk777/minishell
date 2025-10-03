/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/30 21:24:25 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>

static t_token_new	*create_token_by_type(t_lexer_new *lexer)
{
	if (lexer->current_char == '|')
		return (handle_pipe_advanced(lexer));
	else if ((lexer->current_char == ' ' || lexer->current_char == '\t'))
		return (handle_whitespaces_advanced(lexer));
	else if (lexer->current_char == '<' || lexer->current_char == '>')
		return (handle_redirect_advanced(lexer));
	else if (lexer->current_char == '\'' || lexer->current_char == '"')
		return (handle_quotes_advanced(lexer));
	else
		return (handle_word_advanced(lexer, &lexer->first_word_check));
}

static void	handle_post_token_processing(t_lexer_new *lexer, t_token_new *token)
{
	if (token->type == T_CMD || token->type == T_PIPE)
	{
		skip_whitespace_advanced(lexer);
		if (token->type == T_PIPE)
		{
			if (lexer->current_char >= 32 && lexer->current_char <= 127)
				lexer->t_cmd_flag = 1;
		}
	}
}

static t_token_new	*get_next_token(t_lexer_new *lexer)
{
	t_token_new	*token;

	token = NULL;
	lexer->first_word_check = 0;
	if (lexer->current_char == '\0')
		return (NULL);
	token = create_token_by_type(lexer);
	if (!token)
	{
		return (NULL);
	}

	handle_post_token_processing(lexer, token);
	return (token);
}

static int	process_token(t_list **tokens, t_token_new *token)
{
	if (!token)
	{
		printf("Token Error\n");
		return (0);		
	}
	else if (token->value)
	{
		ft_lstadd_back(tokens, ft_lstnew(token));
		return (1);
	}
	return (0);
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
	while (lexer->current_char)
	{
		token = get_next_token(lexer);
		global->echo_flag = 0;
		if (!process_token(&tokens, token))
			return (NULL);
	}
	return (tokens);
}
