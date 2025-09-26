/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_is.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 22:37:15 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/26 22:37:29 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

int	is_command_start(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_WORD || token->type == T_CMD); // Buraya redirectler de eklenecek
}

int	is_pipe_token(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
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

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_WORD || token->type == T_SINGLE_QUOTE || 
            token->type == T_DOUBLE_QUOTE || token->type == T_WHITESPACE || token->type == T_CMD);
}