/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/29 20:02:55 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// void	skip_whitespace_advanced(t_lexer_new *lexer)
// {
// 	while (lexer->current_char == ' ' || lexer->current_char == '\t'
// 		|| lexer->current_char == '\n')
// 		advance_lexer(lexer);
// }

// static int	check_quote_balance(char *input, int *single_quote_count)
// {
// 	int	i;
// 	int	single_quotes;
// 	int	double_quotes;
// 	int	dollar_flag;

// 	i = 0;
// 	single_quotes = 0;
// 	double_quotes = 0;
// 	dollar_flag = 1;
// 	while (input[i])
// 	{
// 		if (input[i] == '\'')
// 			single_quotes++;
// 		else if (input[i] == '"')
// 			double_quotes++;
// 		if (input[i] == '\'' && dollar_flag)
// 			(*single_quote_count)++;
// 		if (input[i] == '$')
// 			dollar_flag = 0;
// 		i++;
// 	}
// 	// Both single and double quotes must be even
// 	return (single_quotes % 2 == 0 && double_quotes % 2 == 0);
// }

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

static t_token_new	*get_next_token(t_lexer_new *lexer, int single_quote_count)
{
	t_token_new	*token;

	/* single_quote_count = 0; */
	/* combined_value = 0; */
	single_quote_count = 0;
	/* next_token = NULL; */
	token = NULL;
	lexer->first_word_check = 0;
	if (lexer->current_char == '\0')
		return (NULL);
	/* İlk token'ı oluştur */
	token = create_token_by_type(lexer);
	if (!token)
		return (NULL);
	/* if (token->type == T_PIPE || token->type == T_HEREDOC
		|| token->type == T_REDIRECT_OUT || token->type == T_REDIRECT_IN) */
	/* pipedan sonrakini t_cmd tipine çevirmek için ve heredocdan sonra boşluğu alıyordu onu atlamak için */
	/* { */
	/* 	if (token->type != T_HEREDOC && token->type != T_REDIRECT_OUT
			&& token->type != T_REDIRECT_IN && token->type != T_PIPE) */
	/* 		advance_lexer(lexer); */
	/* 	if (lexer->current_char == ' ' || lexer->current_char == '\t') */
	/* 		skip_whitespace_advanced(lexer); */
	/* 	if (lexer->current_char >= 32 && lexer->current_char <= 127) */
	/* 		lexer->t_cmd_flag = 1; */
	/* } */
	/* komuttan sonra boşluğu yazıyordu örn: echo selam burada ' selam' bunu engellemek için */
	handle_post_token_processing(lexer, token);
	return (token);
}

// static t_token_new	*get_next_token(t_lexer_new *lexer,int single_quote_count)
// {
// 	t_token_new *token;
// 	t_token_new *next_token;
// 	char *combined_value;

// 	//single_quote_count = 0;
// 	combined_value = 0;
// 	single_quote_count = 0;
// 	next_token = NULL;
// 	token = NULL;
// 	lexer->first_word_check = 0;
// 	if (lexer->current_char == '\0')
// 		return (NULL);

// 	// İlk token'ı oluştur
// 	if (lexer->current_char == '|')
// 		token = handle_pipe_advanced(lexer);
// 	else if ((lexer->current_char == ' ' || lexer->current_char == '\t'))
// 		token = handle_whitespaces_advanced(lexer);
// 	else if (lexer->current_char == '<' || lexer->current_char == '>')
// 		token = handle_redirect_advanced(lexer);
// 	else if (lexer->current_char == '\'' || lexer->current_char == '"')
// 		token = handle_quotes_advanced(lexer);
// 	else
// 		token = handle_word_advanced(lexer, &lexer->first_word_check);

// 	if (!token)
// 		return (NULL);
// 	// if (token->type == T_PIPE || token->type == T_HEREDOC
		//|| token->type == T_REDIRECT_OUT || token->type == T_REDIRECT_IN)
		// pipedan sonrakini t_cmd tipine çevirmek için ve heredocdan sonra boşluğu alıyordu onu atlamak için
// 	// {
// 	// 	if (token->type != T_HEREDOC && token->type != T_REDIRECT_OUT
		//	&& token->type != T_REDIRECT_IN && token->type != T_PIPE)
// 	// 		advance_lexer(lexer);
// 	// 	if (lexer->current_char == ' ' || lexer->current_char == '\t')
// 	// 		skip_whitespace_advanced(lexer);
// 	// 	if (lexer->current_char >= 32 && lexer->current_char <= 127)
// 	// 		lexer->t_cmd_flag = 1;
// 	// }

// 	if (token->type == T_CMD || token->type == T_PIPE)
		// komuttan sonra boşluğu yazıyordu örn: echo selam burada ' selam' bunu engellemek için
// 	{
// 		skip_whitespace_advanced(lexer);
// 		if (token->type == T_PIPE)
// 		{
// 			if (lexer->current_char >= 32 && lexer->current_char <= 127)
// 				lexer->t_cmd_flag = 1;
// 		}

// 	}

// 	return (token);
// }

static int	process_token(t_list **tokens, t_token_new *token)
{
	if (token && token->value)
	{
		ft_lstadd_back(tokens, ft_lstnew(token));
		return (1);
	}
	else if (!token)
	{
		printf("Error: Unclosed quote detected\n");
		return (0);
	}
	return (1);
}

t_list	*tokenize_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;
	t_list		*tokens;
	t_token_new	*token;
	int			single_quote_count;

	single_quote_count = 0;
	if (!check_quote_balance(input, &single_quote_count))
	{
		printf("Error: Unbalanced quotes detected\n");
		return (NULL);
	}
	lexer = init_lexer_advanced(input, global);
	if (!lexer)
		return (NULL);
	tokens = NULL;
	while (lexer->current_char != '\0')
	{
		token = get_next_token(lexer, single_quote_count);
		global->echo_flag = 0;
		if (!process_token(&tokens, token))
			return (NULL);
		if (token && token->value && !token->value[0])
			break ;
	}
	return (tokens);
}

// t_list	*tokenize_advanced(char *input, t_global *global)
// {
// 	t_lexer_new	*lexer;
// 	t_list		*tokens;
// 	t_token_new	*token;
// 	int			single_quote_count;

// 	// Check quote balance before tokenizing
// 	single_quote_count = 0;
// 	if (!check_quote_balance(input, &single_quote_count))
// 	{
// 		printf("Error: Unbalanced quotes detected\n");
// 		return (NULL);
// 	}

// 	lexer = init_lexer_advanced(input, global);
// 	if (!lexer)
// 		return (NULL);
// 	tokens = NULL;
// 	while (lexer->current_char != '\0')
// 	{
// 		token = get_next_token(lexer, single_quote_count);
// 		global->echo_flag = 0;
// 		if (token && token->value)
// 			ft_lstadd_back(&tokens, ft_lstnew(token));
// 		else if (!token)
// 		{
// 			// Handle unclosed quote error
// 			printf("Error: Unclosed quote detected\n");
// 			// free_lexer_advanced(lexer);
// 			return (NULL);
// 		}
// 		else if (token && !token->value[0])
// 			break ;

// 	}
// 	// free_lexer_advanced(lexer);
// 	return (tokens);
// }
