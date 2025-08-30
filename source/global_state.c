/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global_state.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/01/08 12:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

// Global değişken - sinyal handler'lar için
t_global	*g_global = NULL;

t_global	*init_global(char **envp) // bzero YAPILCAK
{
	t_global	*global;

	global = (t_global *)halloc(sizeof(t_global));
	if (!global)
		return (NULL);
	global->tokens = NULL;
	global->commands = NULL;
	global->env_list = init_env_from_envp(envp);
	global->exit_status = 0;
	global->pipe_count = 0;
	global->heredoc_count = 0;
	global->input_line = NULL;
	global->interactive = 1;
	global->in_child = 0;
	global->should_exit = 0;
	
	// Global değişkeni ayarla
	g_global = global;
	
	return (global);
}

void	free_global(t_global *global)
{
	if (!global)
		return ;
	if (global->tokens)
		free_tokens_advanced(&global->tokens);
	if (global->env_list)
		free_env_list(global->env_list);
	if (global->input_line)
		free(global->input_line);
	
	// Global değişkeni temizle
	if (g_global == global)
		g_global = NULL;
		
	free(global);
}

void	update_exit_status(t_global *global, int status)
{
	if (global)
		global->exit_status = status;
}

void	free_token_advanced(void *token)
{
	t_token_new	*t;

	if (!token)
		return ;
	t = (t_token_new *)token;
	if (t->value)
		free(t->value);
	free(t);
}

void	free_tokens_advanced(t_list **tokens)
{
	if (tokens && *tokens)
	{
		ft_lstclear(tokens, free_token_advanced);
		*tokens = NULL;
	}
}
