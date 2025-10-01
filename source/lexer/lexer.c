/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:38:04 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 19:50:10 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

t_lexer_new	*init_lexer_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;

	lexer = (t_lexer_new *)halloc(sizeof(t_lexer_new));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->pos = 0;
	lexer->len = ft_strlen(input);
	lexer->global = global;
	lexer->current_char = '\0';  // ← İNİTİALİZE ET!
    lexer->t_cmd_flag = 0;       // ← İNİTİALİZE ET!
	if (lexer->len > 0)
		lexer->current_char = input[0];
	else
		lexer->current_char = '\0';
	return (lexer);
}

void	advance_lexer(t_lexer_new *lexer)
{
	lexer->pos++;
	if (lexer->pos >= lexer->len)
		lexer->current_char = '\0';
	else
		lexer->current_char = lexer->input[lexer->pos];
}

t_token_new	*create_token_advanced(t_token_types type, char *value)
{
	t_token_new	*token;

	token = (t_token_new *)halloc(sizeof(t_token_new));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	
	if (!token->value)
	{
		// free(token); // büyük ihtimalle bunlar silinecek
		return (NULL);
	}
	token->len = ft_strlen(value);
	token->quote_type = 0;
	token->expanded = 0;
	return (token);
}
void	skip_whitespace_advanced(t_lexer_new *lexer)
{
	while (lexer->current_char == ' ' || lexer->current_char == '\t'
		|| lexer->current_char == '\n')
		advance_lexer(lexer);
}

int	check_quote_balance(char *input)
{
	int	i;
	int	single_quotes;
	int	double_quotes;
	int	dollar_flag;

	i = 0;
	single_quotes = 0;
	double_quotes = 0;
	dollar_flag = 1;
	// while (input[i]) // Herhangi bir tırnak açıldıysa içerisinde ki tırnakları saymaya gerek yok sadece açılan tırnağın kapanıp kapanmadığı checklenecek
	// {
	// 	if (input[i] == '\'')
	// 		single_quotes++;
	// 	else if (input[i] == '"')
	// 		double_quotes++;
	// 	if (input[i] == '\'' && dollar_flag)
	// 		(*single_quote_count)++;
	// 	if (input[i] == '$')
	// 		dollar_flag = 0;
	// 	i++;
	// }
	while (input[i])
	{
		if (input[i] == '\'')
		{
			single_quotes = !single_quotes;
			i++;
			while (input[i])
			{
				if (input[i] == '\'')
					single_quotes = !single_quotes;
				i++;
			}
		}
		else if (input[i] == '"')
		{
			double_quotes = !double_quotes;
			i++;
			while (input[i])
			{
				if (input[i] == '"')
					double_quotes = !double_quotes;
				i++;
			}
		}
		else
			i++;
	}
	
	// Both single and double quotes must be even
	return (single_quotes % 2 == 0 && double_quotes % 2 == 0);
}