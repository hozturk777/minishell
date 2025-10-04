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
#include <readline/readline.h>
#include <unistd.h>
#include <fcntl.h>	

static char	*create_temp_filename(int heredoc_count)
{
	static	long long temp_filename = 1;
	char	*count_str;
	char	*temp1;
	char	*temp2;
	char	*result;

	// pid_str = ft_itoa(getpid()); // KALDIRILCAK
	// temp_filename = 1;
	count_str = ft_itoa(heredoc_count);
	if (!count_str)
	{
		return (NULL);
	}
	temp1 = ft_strjoin("/tmp/heredoc_", ft_lltoa(temp_filename));
	temp2 = ft_strjoin(temp1, "_");
	result = ft_strjoin(temp2, count_str);
	temp_filename++;
	return (result);
}

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

	while (1)
	{
		line = add_garbage(readline("> "));

		if (!line)
			break;

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
	if (redirect->fd > 2)
	{
		register_heredoc_fd(redirect->fd);
		printf("DEBUG: Registered heredoc FD %d\n", redirect->fd);
	}
	return (redirect->fd);
}


static void	preprocess_heredoc_lines(t_redirect *redirect, int fd,
		t_global *global)
{
	char	*line;
	int		pid;

	pid	= fork();

	if (pid == 0)
	{
		setup_child_signals();
		global->in_child = 1;
		while (1)
		{
			line = add_garbage(readline("> "));
	
			if (!line)
				break;
	
			if (ft_strcmp(line, redirect->filename) == 0)
				break ;
			line = expand_with_heredoc(line, global);
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
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

int	preprocess_handle_heredoc(t_redirect *redirect)
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
	preprocess_heredoc_lines(redirect, redirect->fd, global);
	close(redirect->fd);
	redirect->fd = open(temp_filename, O_RDONLY);
	unlink(temp_filename);
	if (redirect->fd > 2)
	{
		register_heredoc_fd(redirect->fd);
	}
	return (redirect->fd);
}