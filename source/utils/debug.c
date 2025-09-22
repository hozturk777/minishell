/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/31 23:50:44 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static char	*get_token_type_name(t_token_types type)
{
	if (type == T_WORD)
		return ("WORD");
	else if (type == T_PIPE)
		return ("PIPE");
	else if (type == T_REDIRECT_IN)
		return ("REDIRECT_IN");
	else if (type == T_REDIRECT_OUT)
		return ("REDIRECT_OUT");
	else if (type == T_APPEND)
		return ("APPEND");
	else if (type == T_HEREDOC)
		return ("HEREDOC");
	else if (type == T_SINGLE_QUOTE)
		return ("SINGLE_QUOTE");
	else if (type == T_DOUBLE_QUOTE)
		return ("DOUBLE_QUOTE");
	else if (type == T_ENV_VAR)
		return ("ENV_VAR");
	else if (type == T_EOF)
		return ("EOF");
	else if (type == T_WHITESPACE)
		return ("T_WHITESPACE");
	return ("UNKNOWN");
}

void	print_tokens_advanced(t_list *tokens)
{
	t_list		*current;
	t_token_new	*token;
	int			i;

	printf("\n" GREEN "=== ADVANCED TOKENS DEBUG ===" RESET "\n");
	current = tokens;
	i = 0;
	while (current)
	{
		token = (t_token_new *)current->content;
		printf("Token[%d]: Type=%-12s Value=$%s$ Len=%d\n", i,
			get_token_type_name(token->type), token->value, token->len);
		current = current->next;
		i++;
	}
	printf(GREEN "=== END TOKENS DEBUG ===" RESET "\n\n");
}

void	debug_print(char *msg)
{
	printf(GREEN "[DEBUG] %s" RESET "\n", msg);
}

int	is_special_char(char c)
{
	if (c == '|' || c == '<' || c == '>' || c == '\'' || c == '"' || c == ' '
		|| c == '\t' || c == '\n')
		return (1);
	return (0);
}

char	*expand_env_var(char *var_name, t_global *global)
{
	t_env	*env_node;

	env_node = find_env_node(global->env_list, var_name);
	if (env_node)
		return (ft_strdup(env_node->value));
	return (ft_strdup(""));
}
