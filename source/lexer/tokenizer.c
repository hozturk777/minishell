/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/21 22:37:30 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// static void	skip_whitespace_advanced(t_lexer_new *lexer)
// {
// 	while (lexer->current_char == ' ' || lexer->current_char == '\t'
// 		|| lexer->current_char == '\n')
// 		advance_lexer(lexer);
// }

static int	check_quote_balance(char *input, int *single_quote_count)
{
	int	i;
	int	single_quotes;
	int	double_quotes;
	int	dollar_flag;

	i = 0;
	single_quotes = 0;
	double_quotes = 0;
	dollar_flag = 1;
	while (input[i])
	{
		if (input[i] == '\'')
			single_quotes++;
		else if (input[i] == '"')
			double_quotes++;
		if (input[i] == '\'' && dollar_flag)
			(*single_quote_count)++;
		if (input[i] == '$')
			dollar_flag = 0;
		i++;
	}
	// Both single and double quotes must be even
	return (single_quotes % 2 == 0 && double_quotes % 2 == 0);
}

static t_token_new	*get_next_token(t_lexer_new *lexer, int single_quote_count)
{
	t_token_new *token;
	t_token_new *next_token;
	char *combined_value;
	
	//single_quote_count = 0;
	// skip_whitespace_advanced(lexer);
	combined_value = 0;
	single_quote_count = 0;
	next_token = NULL;
	token = NULL;
	lexer->first_word_check = 0;
	if (lexer->current_char == '\0')
		return (NULL);

	// İlk token'ı oluştur
	if (lexer->current_char == '|')
		token = handle_pipe_advanced(lexer);
	else if ((lexer->current_char == ' ' || lexer->current_char == '\t'))
		token = handle_whitespaces_advanced(lexer);
	else if (lexer->current_char == '<' || lexer->current_char == '>')
		token = handle_redirect_advanced(lexer);
	else if (lexer->current_char == '\'' || lexer->current_char == '"')
		token = handle_quotes_advanced(lexer);
	else
		token = handle_word_advanced(lexer, &lexer->first_word_check);

	if (!token)
		return (NULL);
	if (lexer->first_word_check == 0 && token->type != T_WHITESPACE) // Komut ile word arasında ki boşluğu atlamak için
	    advance_lexer(lexer);
	

	// Pipe ve redirect operatörleri derhal return edilmeli
	// if (token->type == T_PIPE || token->type == T_REDIRECT_IN || 
	// 	token->type == T_REDIRECT_OUT || token->type == T_APPEND || 
	// 	token->type == T_HEREDOC)
	// 	return (token);
	// if (single_quote_count % 2 == 0)
	// 	// Tek sayıda single quote = literal (expansion yok)
	// 	token->type = T_WORD;
	
	return (token);
}

t_list	*tokenize_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;
	t_list		*tokens;
	t_token_new	*token;
	int			single_quote_count;

	// Check quote balance before tokenizing
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
		if (token && token->value)
			ft_lstadd_back(&tokens, ft_lstnew(token));
		else if (!token)
		{
			// Handle unclosed quote error
			printf("Error: Unclosed quote detected\n");
			free_lexer_advanced(lexer);
			return (NULL);
		}
		else if (token && !token->value[0])
			break ;

	}
	
	free_lexer_advanced(lexer);
	return (tokens);
}
