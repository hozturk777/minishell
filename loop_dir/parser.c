/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 02:50:00 by hsyn              #+#    #+#             */
/*   Updated: 2025/08/22 05:32:34 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

t_command	*parse_tokens_to_commands(t_list *tokens, t_global *global)
{
    t_command	*head;
    t_command	*current_cmd;
    t_list		*token_node;

    head = NULL;
    current_cmd = NULL;
    token_node = tokens;
    while (token_node)
    {
        if (is_command_start(token_node))
        {
            current_cmd = parse_single_command(&token_node, global);
            if (!head)
                head = current_cmd;
            else
                append_command_to_chain(head, current_cmd);
        }
        else if (is_pipe_token(token_node))
        {
            token_node = token_node->next; // Skip pipe token
            // Next token should be the start of new command
        }
        else
            token_node = token_node->next;
    }
    return (head);
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
    while (current && !is_pipe_token(current))
    {
        if (is_redirect_token(current))
            parse_redirection(cmd, &current, global);
        else if (is_word_token(current))
            collect_command_arg(&args_list, current);
        current = current->next;
    }
    cmd->args = convert_list_to_array(args_list);
    *token_node = current;
    ft_lstclear(&args_list, free);
    return (cmd);
}

void	parse_redirection(t_command *cmd, t_list **token_node, t_global *global)
{
    t_redirect		*redirect;
    t_token_new		*token;
    t_token_new		*file_token;

    (void)global;  // Unused parameter for now
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
        redirect->filename = ft_strdup(file_token->value); // burada strdup da garbage collector lazım;
    }
    add_redirect_to_command(cmd, redirect);
}

int	is_command_start(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_WORD || token->type == T_SINGLE_QUOTE || 
            token->type == T_DOUBLE_QUOTE);
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
            token->type == T_DOUBLE_QUOTE);
}

void	collect_command_arg(t_list **args_list, t_list *token_node)
{
    t_token_new	*token;
    char		*arg_copy;

    token = (t_token_new *)token_node->content;
    arg_copy = ft_strdup(token->value);
    ft_lstadd_back(args_list, ft_lstnew(arg_copy));
}

char	**convert_list_to_array(t_list *args_list)
{
    char	**args_array;
    int		size;
    int		i;
    t_list	*current;

    size = ft_lstsize(args_list);
    args_array = halloc(sizeof(char *) * (size + 1));
    if (!args_array)
        return (NULL);
    i = 0;
    current = args_list;
    while (current)
    {
        args_array[i] = ft_strdup((char *)current->content);
        current = current->next;
        i++;
    }
    args_array[i] = NULL;
    return (args_array);
}

void	append_command_to_chain(t_command *head, t_command *new_cmd)
{
    t_command	*current;

    current = head;
    while (current->next)
        current = current->next;
    current->next = new_cmd;
}

void	add_redirect_to_command(t_command *cmd, t_redirect *redirect)
{
    ft_lstadd_back(&cmd->redirections, ft_lstnew(redirect));
}