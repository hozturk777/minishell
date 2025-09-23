/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/23 17:20:46 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
// kurtarma
/* ************************************************************************** */
/*                            MULTIPLE HEREDOC HANDLER                       */
/* ************************************************************************** */

static void	handle_multiple_heredocs(t_command *cmd)
{
	t_list		*current;
	t_redirect	*redirect;
	int			last_heredoc_fd;

	last_heredoc_fd = -1;
	if (!cmd->redirections)
		return ;
	current = cmd->redirections;
	while (current)
	{
		redirect = (t_redirect *)current->content;
		if (redirect && redirect->type == T_HEREDOC)
		{
			if (last_heredoc_fd != -1)
				close(last_heredoc_fd);
			last_heredoc_fd = handle_heredoc(redirect);
		}
		current = current->next;
	}
	if (last_heredoc_fd != -1)
	{
		dup2(last_heredoc_fd, STDIN_FILENO);
		close(last_heredoc_fd);
	}
}

/* ************************************************************************** */
/*                            REDIRECTION SETUP                              */
/* ************************************************************************** */

void	setup_redirections(t_command *cmd)
{
	t_list		*current;
	t_redirect	*redirect;

	if (!cmd || !cmd->redirections)
		return ;
	handle_multiple_heredocs(cmd);
	current = cmd->redirections;
	while (current)
	{
		redirect = (t_redirect *)current->content;
		if (redirect && redirect->type != T_HEREDOC)
			handle_single_redirection(redirect);
		current = current->next;
	}
}

/* ************************************************************************** */
/*                            SINGLE REDIRECTION HANDLER                     */
/* ************************************************************************** */

void	handle_single_redirection(t_redirect *redirect)
{
	int	fd;

	if (redirect->type == T_REDIRECT_IN)
	{
		fd = open(redirect->filename, O_RDONLY);
		if (fd == -1)
		{
			perror(redirect->filename);
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	else if (redirect->type == T_REDIRECT_OUT)
	{
		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			perror(redirect->filename);
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	else if (redirect->type == T_APPEND)
	{
		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1)
		{
			perror(redirect->filename);
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
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
		// if (pid_str)
		//	free(pid_str);
		// if (count_str)
		// 	free(count_str);
		return (NULL);
	}
	temp1 = ft_strjoin("/tmp/heredoc_", pid_str);
	temp2 = ft_strjoin(temp1, "_");
	result = ft_strjoin(temp2, count_str);
	// free(pid_str);
	// free(count_str);
	// free(temp1);
	// free(temp2);
	return (result);
}

int	handle_heredoc(t_redirect *redirect)
{
	char			*line;
	char			*temp_filename;
	// int				fd;
	static int		heredoc_count = 0;

	if (!redirect || redirect->type != T_HEREDOC)
		return (-1);
	temp_filename = create_temp_filename(heredoc_count++);
	if (!temp_filename)
		return (-1);
	redirect->fd = open(temp_filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (redirect->fd == -1)
	{
		// free(temp_filename);
		return (-1);
	}
	write(STDOUT_FILENO, "> ", 2);
	while ((line = readline("")) != NULL)
	{
		if (ft_strcmp(line, redirect->filename) == 0)
		{
			// free(line);
			break ;
		}
		write(redirect->fd, line, ft_strlen(line));
		write(redirect->fd, "\n", 1);
		// free(line);
		write(STDOUT_FILENO, "> ", 2);
	}
	close(redirect->fd);
	redirect->fd = open(temp_filename, O_RDONLY);
	unlink(temp_filename);
	// free(temp_filename);
	return (redirect->fd);
}

/* ************************************************************************** */
