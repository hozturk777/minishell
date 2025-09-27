/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 00:03:43 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/28 00:03:57 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../lib/minishell.h"

/* ************************************************************************** */
/*                            HEREDOC IMPLEMENTATION                         */
/* ************************************************************************** */

static char	*create_temp_filename(int heredoc_count)
{
	char	*pid_str;
	char	*count_str;
	char	*temp1;
	char	*temp2;
	char	*result;

	pid_str = ft_itoa(getpid());
	count_str = ft_itoa(heredoc_count);
	if (!pid_str || !count_str)
	{
		return (NULL);
	}
	temp1 = ft_strjoin("/tmp/heredoc_", pid_str);
	temp2 = ft_strjoin(temp1, "_");
	result = ft_strjoin(temp2, count_str);
	return (result);
}

/* ************************************************************************** */
/*                            PIPELINE FD SETUP                              */
/* ************************************************************************** */

void	setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)
{
	if (prev_fd != 0)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (cmd->next)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
}


static void	process_heredoc_lines(t_redirect *redirect, int fd,
		t_global *global)
{
	char	*line;

	while ((line = add_garbage(readline("> "))) != NULL)
	{
		if (ft_strcmp(line, redirect->filename) == 0)
			break ;
		line = expand_with_heredoc(line, global);
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
	}
}

int	handle_heredoc(t_redirect *redirect)
{
	char		*temp_filename;
	static int	heredoc_count = 0;
	t_global	*global;

	global = get_global();
	if (!redirect || redirect->type != T_HEREDOC)
		return (-1);
	temp_filename = create_temp_filename(heredoc_count++);
	if (!temp_filename)
		return (-1);
	redirect->fd = open(temp_filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (redirect->fd == -1)
		return (-1);
	process_heredoc_lines(redirect, redirect->fd, global);
	close(redirect->fd);
	redirect->fd = open(temp_filename, O_RDONLY);
	unlink(temp_filename);
	return (redirect->fd);
}
