/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 02:50:00 by hsyn              #+#    #+#             */
/*   Updated: 2025/09/28 00:10:48 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>

t_command	*parse_tokens_to_commands(t_list *tokens, t_global *global)
{
    t_command	*head;
    t_command	*current;
    t_list		*token_node;

    head = NULL;
    current = NULL;
    token_node = tokens;
	
    while (token_node)
    {
        if (is_command_start(token_node))
        {
            current = parse_single_command(&token_node, global);
			if (!current)
				return (NULL);
            if (!head)
                head = current;
            else
                append_command_to_chain(head, current);
        }
        else
            token_node = token_node->next;
    }
		
    return (head);
}
int	check_syntax(t_list **token_node)
{
	t_list		*token_temp;
	t_token_new	*token_test;

	token_temp = *token_node;
	while (token_temp)
	{
	    token_test = (t_token_new *)token_temp->content;
		if (token_test->type == T_REDIRECT_IN || token_test->type == T_REDIRECT_OUT || token_test->type == T_APPEND)
		{
			token_temp = token_temp->next;
			if (!token_temp)
			{
				printf("minishell: syntax error near unexpected token `newline'\n");
				return (1);
			}
			token_test = (t_token_new *)token_temp->content;
			if (token_test->type == T_WHITESPACE)
			{
				token_temp = token_temp->next;
				token_test = (t_token_new *)token_temp->content;
			}
			if (token_test->type != T_WORD && token_test->type != T_CMD)
			{
				printf("minishell: syntax error near unexpected token `%s'\n", token_test->value);
				return (1);
			}
			continue;
		}
		else if (token_test->type == T_HEREDOC)
		{
			token_temp = token_temp->next;
			if (!token_temp)
			{
				printf("minishell: syntax error near unexpected token `newline'\n");
				return (1);
			}
			token_test = (t_token_new *)token_temp->content;
			if (token_test->type != T_WORD && token_test->type != T_CMD)
			{
				printf("minishell: syntax error near unexpected token `%s'\n", token_test->value);
				return (1);
			}
			continue;
		}
		
		token_temp = token_temp->next;
	}
	return (0);
}

static void	parse_redirection(t_command *cmd, t_list **token_node)
{
    t_redirect		*redirect;
    t_token_new		*token;
    t_token_new		*file_token;

    token = (t_token_new *)(*token_node)->content;
    redirect = halloc(sizeof(t_redirect));
    if (!redirect)
        return ;
    redirect->type = token->type;
    redirect->fd = -1;
    redirect->next = NULL;
    *token_node = (*token_node)->next;
    if (*token_node)
    {
        file_token = (t_token_new *)(*token_node)->content;
		if (file_token->type == T_WHITESPACE)
		{
			*token_node = (*token_node)->next;
	        file_token = (t_token_new *)(*token_node)->content;
		}
        redirect->filename = ft_strdup(file_token->value);
    }
    add_redirect_to_command(cmd, redirect);
}

t_command	*parse_single_command(t_list **token_node, t_global *global)
{
    t_command	*cmd;
    t_list		*args_list;
    t_list		*current;

    cmd = create_command();
    if (!cmd)
        return (NULL);
    args_list = NULL;
    current = *token_node;
	if (check_syntax(token_node))
	{
		return (NULL);
	}
    while (current && !is_pipe_token(current))
    {
        if (is_redirect_token(current))
            parse_redirection(cmd, &current);
        else if (is_word_token(current))
            collect_command_arg(&args_list, current);
		current = current->next;
    }
    cmd->args = convert_list_to_array(args_list);
    *token_node = current;
    ft_lstclear(&args_list, free);
    expand_command_args(cmd, global);
    
    return (cmd);
}


void	collect_command_arg(t_list **args_list, t_list *token_node)
{
    t_token_new	*token;
    char		*arg_copy;
    char		*temp;

    token = (t_token_new *)token_node->content;
    if (token->type == T_SINGLE_QUOTE)
    {
        temp = ft_strjoin("\'", token->value);
        arg_copy = ft_strjoin(temp, "\'");
    }
    else if (token->type == T_DOUBLE_QUOTE)
    {
        temp = ft_strjoin("\"", token->value);
        arg_copy = ft_strjoin(temp, "\"");
    }
    else
        arg_copy = ft_strdup(token->value);
    ft_lstadd_back(args_list, ft_lstnew(arg_copy));
}
