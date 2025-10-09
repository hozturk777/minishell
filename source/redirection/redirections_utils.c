/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/28 00:08:09 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <readline/readline.h>
#include <unistd.h>

static void	write_heredoc(char *line, t_global *global, int fd)
{
	line = expand_with_heredoc(line, global);
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
}

void	preprocess_heredoc_lines(t_redirect *redirect, int fd,
		t_global *global)
{
	char	*line;
	int		pid;

	pid = fork();
	if (pid == 0)
	{
		setup_child_signals();
		global->in_child = 1;
		while (1)
		{
			line = add_garbage(readline("> "));
			if (!line)
				break ;
			if (ft_strcmp(line, redirect->filename) == 0)
				break ;
			write_heredoc(line, global, fd);
		}
		close(redirect->fd);
		cleanup_and_exit();
		exit (0);
	}
	else if (pid > 0)
	{
		global->exit_status = wait_for_redirect_process(pid);
	}
}
