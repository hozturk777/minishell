/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/24 12:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"

/* ************************************************************************** */
/*                            REDIRECTION SETUP                              */
/* ************************************************************************** */

void	setup_redirections(t_command *cmd)
{
	t_list		*current;
	t_redirect	*redirect;

	if (!cmd || !cmd->redirections)
		return ;
	current = cmd->redirections;
	while (current)
	{
		redirect = (t_redirect *)current->content;
		if (redirect)
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
	else if (redirect->type == T_HEREDOC)
	{
		fd = handle_heredoc(redirect->filename);
		if (fd == -1)
		{
			perror("heredoc");
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
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

char	*generate_temp_filename(void)
{
	static int	counter = 0;
	char		*num_str;
	char		*temp_name;
	char		*final_name;

	num_str = ft_itoa(counter++);
	if (!num_str)
		return (NULL);
	temp_name = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	free(num_str);
	if (!temp_name)
		return (NULL);
	final_name = ft_strjoin(temp_name, ".tmp");
	free(temp_name);
	return (final_name);
}

int	handle_heredoc(char *delimiter)
{
	char	*temp_filename;
	char	*line;
	int		temp_fd;

	temp_filename = generate_temp_filename();
	if (!temp_filename)
		return (-1);
	temp_fd = open(temp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (temp_fd == -1)
	{
		free(temp_filename);
		return (-1);
	}
	printf("> ");
	fflush(stdout);
	while ((line = readline("")) != NULL)
	{
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(temp_fd, line, ft_strlen(line));
		write(temp_fd, "\n", 1);
		free(line);
		printf("> ");
		fflush(stdout);
	}
	close(temp_fd);
	temp_fd = open(temp_filename, O_RDONLY);
	unlink(temp_filename);
	free(temp_filename);
	return (temp_fd);
}

/* ************************************************************************** */
