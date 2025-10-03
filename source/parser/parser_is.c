/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_is.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 00:09:39 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/28 00:09:56 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

int	is_command_start(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_WORD || token->type == T_CMD || token->type == T_HEREDOC || token->type == T_REDIRECT_OUT || token->type == T_REDIRECT_IN || token->type == T_APPEND || token->type == T_DOUBLE_QUOTE || token->type == T_SINGLE_QUOTE);
}

int	is_pipe_token(t_list *token_node)
{
    t_token_new	*token;
	t_global *global;

	global = get_global();
    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
	if (token->type == T_PIPE)
		global->echo_flag = 0;
    return (token->type == T_PIPE);
}

int	is_redirect_token(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_REDIRECT_IN || token->type == T_REDIRECT_OUT
        || token->type == T_APPEND || token->type == T_HEREDOC);
}

int	is_word_token(t_list *token_node)
{
    t_token_new	*token;
	t_global	*global;

	global = get_global();
    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
	if (!ft_strcmp(token->value, "echo"))
		global->echo_flag = 1;	

	if (global->echo_flag)
	{
		return (token->type == T_WORD || token->type == T_SINGLE_QUOTE || 
		token->type == T_DOUBLE_QUOTE || token->type == T_WHITESPACE || token->type == T_CMD);
	}
	
	return (token->type == T_WORD || token->type == T_SINGLE_QUOTE || 
            token->type == T_DOUBLE_QUOTE || token->type == T_CMD);
}
