/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abakirca <abakirca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/10/01 18:45:21 by abakirca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>

int	preprocess_heredocs(t_command *commands, t_global *global)
{
	t_command	*current;
	t_list		*redirect_node;
	t_redirect	*redirect;

	(void)global;
	current = commands;
	while (current)
	{
		if (current->redirections)
		{
			redirect_node = current->redirections;
			while (redirect_node)
			{
				redirect = (t_redirect *)redirect_node->content;
				if (redirect && redirect->type == T_HEREDOC)
				{
					redirect->fd = preprocess_handle_heredoc(redirect);
					if (redirect->fd == -1)
						return (-1);
				}
				redirect_node = redirect_node->next;
			}
		}
		current = current->next;
	}
	return (0);
}

static void	close_unused_heredoc_fds(t_command *cmd)
{
	t_list		*node;
	t_redirect	*redirect;

	if (!cmd->redirections)
		return ;
	
	node = cmd->redirections;
	while (node)
	{
		redirect = (t_redirect *)node->content;
		if (redirect && redirect->type == T_HEREDOC && redirect->fd > 2)
		{
			close(redirect->fd);
			redirect->fd = -1;
		}
		node = node->next;
	}
}

static int	redirect_check(t_command *command)
{
	t_redirect	*redirect;
	t_list		*node;


	node = command->redirections;
	while (node)
	{
		redirect = (t_redirect *)node->content;
		while (redirect)
		{
			if (redirect->type != T_HEREDOC)
				return (1);
			redirect = redirect->next;
		}
		node = node->next;
	}
	return (0);
}

static int	heredoc_check(t_command	*command)
{
	t_command	*command_temp;
	t_redirect	*redirect;
	t_list	*node;

	command_temp = command->next;
	if (!command_temp)
		return (0);
	node = command_temp->redirections;
	redirect = NULL;
	while (command_temp)
	{
		node = command_temp->redirections;
		if (node)
			redirect = (t_redirect *)node->content;
		if (redirect)
		{
			while (redirect)
			{
				if (redirect->type == T_HEREDOC)
					return (1);
				redirect = redirect->next;
			}
		}
		command_temp = command_temp->next;
	}
	return (0);
}

pid_t	execute_pipeline_command_async(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
	pid_t		pid;
	char		*path;
	int			exit_num;
	t_redirect	*redirect;
	t_list		*node;
	int			originals[2];

	node = cmd->redirections;
	pid = fork();
	exit_num = 0;
	if (pid == 0)
	{
		setup_child_signals();
		global->in_child = 1;
		originals[0] = dup(STDIN_FILENO);
		originals[1] = dup(STDOUT_FILENO);
		setup_pipeline_fds(cmd, prev_fd, pipe_fd);
		if(setup_redirections(cmd))
		{
			close_unused_heredoc_fds(cmd);
			close(originals[0]);
			close(originals[1]);
			cleanup_and_exit();
			exit(2);
		}
		close_unused_heredoc_fds(cmd);
		close(originals[0]);
		close(originals[1]);
		close_all_heredoc_fds();

		if (is_builtin(cmd->args[0]))
		{
			if (heredoc_check(cmd) && !redirect_check(cmd) && !isatty(STDOUT_FILENO))
			{
				cleanup_and_exit();
				exit(0);
			}
			else
			{
				execute_builtin(cmd, global, originals, 0);
				exit_num = global->exit_status;
				cleanup_and_exit();
				exit(exit_num);
			}
		}
		path = find_command_path(cmd->args[0], global->env_list);
        if (!path)
        {
			while (node)
			{
				redirect = (t_redirect *)node->content;
				if (redirect->type == T_HEREDOC)
				{
					close(originals[0]);
					close(originals[1]);
					cleanup_and_exit();
					exit(0);
				}
				node = node->next;
			}
            printf("minishell: %s: command not found\n", cmd->args[0]);
			close(originals[0]);
			close(originals[1]);
            cleanup_and_exit();
            exit(127);
        }
		execve(path, cmd->args, env_list_to_array(global->env_list));
		perror("execve");
		cleanup_and_exit();
		close(originals[0]);
		close(originals[1]);
		exit(127);
	}
	else if (pid > 0)
		return (pid);
	else
	{
		perror("fork");
		return (-1);
	}
}
