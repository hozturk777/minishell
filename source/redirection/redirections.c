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

// kurtarma
/* ************************************************************************** */
/*                            MULTIPLE HEREDOC HANDLER                       */
/* ************************************************************************** */

static int	process_heredoc_redirects(t_list *cur)
{
	t_redirect	*redirect;
	int			last_heredoc_fd;

	last_heredoc_fd = -1;
	while (cur)
	{
		redirect = (t_redirect *)cur->content;
		if (redirect && redirect->type == T_HEREDOC)
		{
			// Önceki heredoc FD'sini kapat (sadece en son geçerli)
			if (last_heredoc_fd != -1 && last_heredoc_fd != redirect->fd)
				close(last_heredoc_fd);
			
			if (redirect->fd > 0)
			{
				last_heredoc_fd = redirect->fd;
				// redirect->fd'yi parent'ta kapatmak için -1 yapma!
			}
			else
				last_heredoc_fd = handle_heredoc(redirect);
		}
		cur = cur->next;
	}
	return (last_heredoc_fd);
}

void	handle_multiple_heredocs(t_command *cmd)
{
	int	fd;

	if (!cmd->redirections)
		return ;
	fd = process_heredoc_redirects(cmd->redirections);
	if (fd != -1)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

// void	handle_multiple_heredocs(t_command *cmd)
// {
// 	t_list		*current;
// 	t_redirect	*redirect;
// 	int			last_heredoc_fd;

// 	last_heredoc_fd = -1;
// 	if (!cmd->redirections)
// 		return ;
// 	current = cmd->redirections;
// 	while (current)
// 	{
// 		redirect = (t_redirect *)current->content;
// 		if (redirect && redirect->type == T_HEREDOC)
// 		{
// 			if (last_heredoc_fd != -1)
// 				close(last_heredoc_fd);
// 			// Eğer pre-processed ise (pipeline durumu), fd'yi kullan
// 			if (redirect->fd > 0)
// 				last_heredoc_fd = redirect->fd;
// 			else
// 				// Değilse (normal durumda), heredoc'u işle
// 				last_heredoc_fd = handle_heredoc(redirect);
// 		}
// 		current = current->next;
// 	}
// 	if (last_heredoc_fd != -1)
// 	{
// 		dup2(last_heredoc_fd, STDIN_FILENO);
// 		close(last_heredoc_fd);
// 	}
// }

// /* ************************************************************************** */
// /*                            REDIRECTION SETUP                              */
// /* ************************************************************************** */

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
		{
			handle_single_redirection(redirect);
		}
		current = current->next;
	}
}

// /* ************************************************************************** */
// /*                            SINGLE REDIRECTION HANDLER                     */
// /* ************************************************************************** */

static int	open_redirection_file(t_redirect *redirect)
{
	int	fd;

	if (redirect->type == T_REDIRECT_IN)
		fd = open(redirect->filename, O_RDONLY);
	else if (redirect->type == T_REDIRECT_OUT)
		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redirect->type == T_APPEND)
		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = -1;
	if (fd == -1)
	{
		perror(redirect->filename);
		cleanup_and_exit();
		exit(1);
	}
	return (fd);
}

void	handle_single_redirection(t_redirect *redirect)
{
	int	fd;

	fd = open_redirection_file(redirect);
	if (redirect->type == T_REDIRECT_IN)
		dup2(fd, STDIN_FILENO);
	else
		dup2(fd, STDOUT_FILENO);
	close(fd);
}

// // void	handle_single_redirection(t_redirect *redirect)
// // {
// // 	int	fd;

// // 	if (redirect->type == T_REDIRECT_IN)
// // 	{
// // 		fd = open(redirect->filename, O_RDONLY);
// // 		if (fd == -1)
// // 		{
// // 			perror(redirect->filename);
// // 			clear_garbage(); /*!!!!!!!!!!!!!!!!!!!!!!!!!burası önemli mi neden böyle ??!!!!!!!!*/
// // 			exit(1);
// // 		}
// // 		dup2(fd, STDIN_FILENO);
// // 		close(fd);
// // 	}
// // 	else if (redirect->type == T_REDIRECT_OUT)
// // 	{
// // 		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// // 		if (fd == -1)
// // 		{
// // 			perror(redirect->filename);
// // 			exit(1);
// // 		}
// // 		dup2(fd, STDOUT_FILENO);
// // 		close(fd);
// // 	}
// // 	else if (redirect->type == T_APPEND)
// // 	{
// // 		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
// // 		if (fd == -1)
// // 		{
// // 			perror(redirect->filename);
// // 			exit(1);
// // 		}
// // 		dup2(fd, STDOUT_FILENO);
// // 		close(fd);
// // 	}
// // }

// /* ************************************************************************** */
// /*                            PIPELINE FD SETUP                              */
// /* ************************************************************************** */

// void	setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)
// {
// 	if (prev_fd != 0)
// 	{
// 		dup2(prev_fd, STDIN_FILENO);
// 		close(prev_fd);
// 	}
// 	if (cmd->next)
// 	{
// 		close(pipe_fd[0]);
// 		dup2(pipe_fd[1], STDOUT_FILENO);
// 		close(pipe_fd[1]);
// 	}
// }

// /* ************************************************************************** */
// /*                            HEREDOC IMPLEMENTATION                         */
// /* ************************************************************************** */

// static char	*create_temp_filename(int heredoc_count)
// {
// 	char	*pid_str;
// 	char	*count_str;
// 	char	*temp1;
// 	char	*temp2;
// 	char	*result;

// 	pid_str = ft_itoa(getpid());
// 	count_str = ft_itoa(heredoc_count);
// 	if (!pid_str || !count_str)
// 	{
// 		// if (pid_str)
// 		//	free(pid_str);
// 		// if (count_str)
// 		// 	free(count_str);
// 		return (NULL);
// 	}
// 	temp1 = ft_strjoin("/tmp/heredoc_", pid_str);
// 	temp2 = ft_strjoin(temp1, "_");
// 	result = ft_strjoin(temp2, count_str);
// 	// free(pid_str);
// 	// free(count_str);
// 	// free(temp1);
// 	// free(temp2);
// 	return (result);
// }

// int	handle_heredoc(t_redirect *redirect)
// {
// 	char		*line;
// 	char		*temp_filename;
// 	static int	heredoc_count = 0;
// 	t_global	*global;

// 	global = get_global();
// 	if (!redirect || redirect->type != T_HEREDOC)
// 		return (-1);
// 	temp_filename = create_temp_filename(heredoc_count++);
// 	if (!temp_filename)
// 		return (-1);
// 	redirect->fd = open(temp_filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
// 	if (redirect->fd == -1)
// 		return (-1);
// 	while ((line = add_garbage(readline("> "))) != NULL)
// 	{
// 		if (ft_strcmp(line, redirect->filename) == 0)
// 			break ;
// 		// if (line[0] == '$')
// 		line = expand_with_heredoc(line, global);
// 		write(redirect->fd, line, ft_strlen(line));
// 		write(redirect->fd, "\n", 1);
// 	}
// 	close(redirect->fd);
// 	redirect->fd = open(temp_filename, O_RDONLY);
// 	unlink(temp_filename);
// 	return (redirect->fd);
// }

// /* ************************************************************************** */
