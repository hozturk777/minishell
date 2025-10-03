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
				last_heredoc_fd = redirect->fd;
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
		perror(redirect->filename);
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
	if (fd > 2)
	{
		close(fd);
	}	
}
