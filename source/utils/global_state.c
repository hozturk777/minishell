/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global_state.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/27 23:06:42 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// Global değişken - sinyal handler'lar için
// t_global	*g_global = NULL;

static void	update_shlvl(t_env *env)
{
	int	value;

	value = ft_atoi(get_env_value(env, "SHLVL"));
	value++;
	set_env_var(get_global(), "SHLVL", ft_itoa(value));
}
 // bzero YAPILCAK
t_global	*init_global(char **envp, t_global *global)
{
	int	i;

	// global = (t_global *)halloc(sizeof(t_global));
	if (!global)
		return (NULL);
	global->tokens = NULL;
	global->commands = NULL;
	global->env_list = init_env_from_envp(envp);
	update_shlvl(global->env_list);
	global->exit_status = 0;
	global->pipe_count = 0;
	global->heredoc_count = 0;
	global->input_line = NULL;
	global->interactive = 1;
	global->in_child = 0;
	global->should_exit = 0;
	global->heredoc_fd_count = 0;
	i = 0;
	while (i < MAX_HEREDOC_FDS)
		global->heredoc_fds[i++] = -1;

	// Global değişkeni ayarla
	// g_global = global;

	return (global);
}

void	update_exit_status(t_global *global, int status)
{
	if (global)
		global->exit_status = status;
}

void	register_heredoc_fd(int fd)
{
	t_global	*global;

	global = get_global();
	if (!global || fd <= 2)
		return ;
	if (global->heredoc_fd_count < MAX_HEREDOC_FDS)
	{
		global->heredoc_fds[global->heredoc_fd_count] = fd;
		global->heredoc_fd_count++;
		debug_print("Registered heredoc FD");
	}
	else
	{
		debug_print("WARNING: Max heredoc FDs reached, cannot register more");
		// FD'yi yine de kapat ki leak olmasın
		close(fd);
	}
}

void	close_all_heredoc_fds(void)
{
	t_global	*global;
	int			i;

	global = get_global();
	if (!global)
		return ;
	
	i = 0;
	while (i < global->heredoc_fd_count)
	{
		if (global->heredoc_fds[i] > 2)
		{
			close(global->heredoc_fds[i]);
			global->heredoc_fds[i] = -1;
		}
		i++;
	}
	global->heredoc_fd_count = 0;
}

void	cleanup_and_exit(void)
{
	close_all_heredoc_fds();
	clear_garbage();
}
