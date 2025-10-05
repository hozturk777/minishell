/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:55:56 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/05 20:57:17 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>

int	check_syntax_redirect(t_list **token_temp, t_token_new **token_test)
{
	(*token_temp) = (*token_temp)->next;
	if (!(*token_temp))
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (1);
	}
	*token_test = (t_token_new *)(*token_temp)->content;
	if ((*token_test)->type == T_WHITESPACE)
	{
		*token_temp = (*token_temp)->next;
		*token_test = (t_token_new *)(*token_temp)->content;
	}
	if ((*token_test)->type != T_WORD && (*token_test)->type != T_CMD)
	{
		printf("minishell: syntax error near unexpected token ");
		printf("`%s'\n", (*token_test)->value);
		return (1);
	}
	return (0);
}

int	check_syntax_heredoc(t_list **token_temp, t_token_new **token_test)
{
	(*token_temp) = (*token_temp)->next;
	if (!(*token_temp))
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (1);
	}
	*token_test = (t_token_new *)(*token_temp)->content;
	if ((*token_test)->type != T_WORD && (*token_test)->type != T_CMD)
	{
		printf("minishell: syntax error near unexpected token ");
		printf("`%s'\n", (*token_test)->value);
		return (1);
	}
	return (0);
}
