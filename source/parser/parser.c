/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 02:50:00 by hsyn              #+#    #+#             */
/*   Updated: 2025/09/21 18:00:44 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

t_command	*parse_tokens_to_commands(t_list *tokens, t_global *global)
{
    t_command	*head;
    t_command	*current;
    t_list		*token_node;
	// t_token_new *new_token = (t_token_new *)tokens->content;

    head = NULL;
    current = NULL;
    token_node = tokens;
	// while (tokens->next)
	// {
	// 	printf("HELLOO, %s\n", (char *)new_token->value);
	// 	tokens = tokens->next;
	// 	new_token = (t_token_new *)tokens->content;
	// }
	
    while (token_node) // Tüm tokenları gezecek
    {
		// printf("HELLOO, %s\n", (char *)new_token->value);
        if (is_command_start(token_node)) // Token word ise
        {
            current = parse_single_command(&token_node, global);
            if (!head)
                head = current;
            else
                append_command_to_chain(head, current);
        }
        else // Token word değil ise diğer token'a geç
            token_node = token_node->next;
    }
	
    return (head);
}

t_command	*parse_single_command(t_list **token_node, t_global *global)
{
    t_command	*cmd;
    t_list		*args_list;
    t_list		*current;

    cmd = create_command(); // Command list initialize
    if (!cmd) // Buraya bizim error_check yapılabilir
        return (NULL);
    args_list = NULL;
    current = *token_node;
    while (current && !is_pipe_token(current)) // Pipelar arasını word ve redirectleri ayırma işlemi
    {
        if (is_redirect_token(current)) // Redirect var ise yönlendirmeyi yapıyor
            parse_redirection(cmd, &current, global); // redirect var ise cmd.redirections'a ekler
        else if (is_word_token(current)) // Wordlari de listeye ekliyor
            collect_command_arg(&args_list, current); // Wordleri arg_list içerisine ekler
		current = current->next;
    }
    cmd->args = convert_list_to_array(args_list); // oluşan arg_list'i cmd.args'e ekler
    *token_node = current; // token_node'u eğer command line bittiyse NULL pipe'a denk geldiyse yeni command'a geçer
    ft_lstclear(&args_list, free);
    
    // Variable expansion işlemi
    expand_command_args(cmd, global); // Dolar gelirse expand işlemi yapıyor ve tırnak işlemleri burada FİXlenecek ama!!
    
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
    return (token->type == T_WORD);
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
    char		*temp;

    token = (t_token_new *)token_node->content;
    
    // Handle different token types
    if (token->type == T_SINGLE_QUOTE)
    {
        // Mark single quoted strings with special prefix to prevent expansion
        temp = ft_strjoin("__SINGLE_QUOTE__", token->value);
        arg_copy = ft_strjoin(temp, "__END_SINGLE_QUOTE__");
        // free(temp);
    }
    else if (token->type == T_DOUBLE_QUOTE)
    {
        // Double quoted strings should be marked for expansion by keeping quotes
        temp = ft_strjoin("\"", token->value);
        arg_copy = ft_strjoin(temp, "\"");
        // free(temp);
    }
    else
    {
        // Regular words
        arg_copy = ft_strdup(token->value);
    }
    
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