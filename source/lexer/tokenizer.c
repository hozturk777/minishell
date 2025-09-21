/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/21 17:32:33 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static void	skip_whitespace_advanced(t_lexer_new *lexer)
{
	while (lexer->current_char == ' ' || lexer->current_char == '\t'
		|| lexer->current_char == '\n')
		advance_lexer(lexer);
}

static int	check_quote_balance(char *input)
{
	int	i;
	int	single_quotes;
	int	double_quotes;

	i = 0;
	single_quotes = 0;
	double_quotes = 0;
	while (input[i])
	{
		if (input[i] == '\'')
			single_quotes++;
		else if (input[i] == '"')
			double_quotes++;
		i++;
	}
	// Both single and double quotes must be even
	return (single_quotes % 2 == 0 && double_quotes % 2 == 0);
}

static t_token_new	*get_next_token(t_lexer_new *lexer)
{
	t_token_new *token;
	t_token_new *next_token;
	char *combined_value;
	
	skip_whitespace_advanced(lexer);
	if (lexer->current_char == '\0')
		return (NULL);
		
	// İlk token'ı oluştur
	if (lexer->current_char == '|')
		token = handle_pipe_advanced(lexer);
	else if (lexer->current_char == '<' || lexer->current_char == '>')
		token = handle_redirect_advanced(lexer);
	else if (lexer->current_char == '\'' || lexer->current_char == '"')
		token = handle_quotes_advanced(lexer);
	else
		token = handle_word_advanced(lexer);
	
	if (!token)
		return (NULL);
		
	// Adjacent token kontrolü - boşluk olmadan devam eden token'lar var mı?
	while (lexer->current_char != '\0' && lexer->current_char != ' ' 
		&& lexer->current_char != '\t' && lexer->current_char != '\n'
		&& lexer->current_char != '|' && lexer->current_char != '<' 
		&& lexer->current_char != '>')
	{
		// Sonraki karakter tokenizable mı kontrol et
		if (lexer->current_char == '\'' || lexer->current_char == '"')
			next_token = handle_quotes_advanced(lexer);
		else if (ft_isalnum(lexer->current_char) || lexer->current_char == '_' 
			|| lexer->current_char == '$' || lexer->current_char == '/' 
			|| lexer->current_char == '.')
			next_token = handle_word_advanced(lexer);
		else
			break; // Tokenizable değilse dur
			
		if (!next_token)
			break;
			
		// İki token'ı birleştir
		combined_value = ft_strjoin(token->value, next_token->value);
		token->value = combined_value; // Halloc kullanıyoruz, eski value'yu free etme
		token->len = ft_strlen(combined_value);
		
		// Mixed token type durumu - expansion için T_WORD yap
		if (token->type != next_token->type)
			token->type = T_WORD;
	}
	
	return (token);
}

t_list	*tokenize_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;
	t_list		*tokens;
	t_token_new	*token;

	// Check quote balance before tokenizing
	if (!check_quote_balance(input))
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
		token = get_next_token(lexer);
		if (token && token->value && token->value[0])
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
