/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:47:26 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 20:49:55 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static int	execute_command_in_child(t_command *cmd, t_global *global);

/* ************************************************************************** */
/*                            MAIN EXECUTOR FUNCTION                         */
/* ************************************************************************** */

int	execute_commands(t_command *commands, t_global *global)
{
	if (!commands)
		return (0);
	if (commands->next)
		return (execute_pipeline(commands, global)); // BURADAYIZ
	else
		return (execute_single_command(commands, global));
}

/* ************************************************************************** */
/*                            SINGLE COMMAND EXECUTION                       */
/* ************************************************************************** */

int	execute_single_command(t_command *cmd, t_global *global)
{

	if ((!cmd || !cmd->args || !cmd->args[0]) && cmd->redirections)
	{
		return (execute_redirect_command(cmd, global));
	}
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);

	// Single command'da da heredoc varsa child process'te çalıştır
	if (cmd->redirections)
	{
		t_list *redirect_node = cmd->redirections;
		while (redirect_node)
		{
			t_redirect *redirect = (t_redirect *)redirect_node->content;
			if (redirect && redirect->type == T_HEREDOC)
			{
				// Heredoc var, child process'te çalıştır
				return (execute_command_in_child(cmd, global));
			}
			redirect_node = redirect_node->next;
		}
	}
	
	if (is_builtin(cmd->args[0]))
		return (execute_builtin(cmd, global));
	return (execute_external_command(cmd, global));
}

/* ************************************************************************** */
/*                            EXTERNAL COMMAND EXECUTION                     */
/* ************************************************************************** */


static void	handle_redirect_child_process(t_command *cmd, t_global *global)
{
    /* Setup child signal handlers */
    setup_child_signals();
    global->in_child = 1;
    /* Setup redirections for the command */
    setup_redirections(cmd);
    /* Exit after setup - redirect only command */
    cleanup_and_exit();
    exit(0);
}

static int	wait_for_redirect_process(pid_t pid)
{
	int	status;

    /* Wait for redirect process to complete */
    waitpid(pid, &status, 0);
    /* Handle signal termination */
    if (WIFSIGNALED(status))
    {
		int signal_num = WTERMSIG(status);
        /* Return appropriate exit code for signals */
        if (signal_num == SIGINT)
		return (130);
        else if (signal_num == SIGQUIT)
		return (131);
        else
		return (128 + signal_num);
    }
    /* Return normal exit status */
    return (WEXITSTATUS(status));
}

int	execute_redirect_command(t_command *cmd, t_global *global)
{
	pid_t	pid;

	/* Fork child process for redirect command */
	pid = fork();
	if (pid == 0)
	{
		/* Child process handles redirect execution */
		handle_redirect_child_process(cmd, global);
	}
	else if (pid > 0)
	{
		/* Parent process waits for child completion */
		return (wait_for_redirect_process(pid));
	}
	/* Fork failed */
	return (1);
}

/* ************************************************************************** */
/*                            CHILD PROCESS EXECUTION                        */
/* ************************************************************************** */

static int	execute_command_in_child(t_command *cmd, t_global *global)
{
	pid_t	pid;
	int		status;
	int		exit_status;

	pid = fork();
	if (pid == 0)
	{
		// Child process - heredoc'ları güvenle işleyebiliriz
		setup_child_signals();
		global->in_child = 1;
		
		if (cmd->redirections)
			setup_redirections(cmd);
		
		if (is_builtin(cmd->args[0]))
		{
			exit_status = execute_builtin(cmd, global);
			cleanup_and_exit();
			exit(exit_status);
		}
		else
		{
			char *path = find_command_path(cmd->args[0], global->env_list);
			if (!path)
			{
				write(STDERR_FILENO, "minishell: ", 11);
				write(STDERR_FILENO, cmd->args[0], ft_strlen(cmd->args[0]));
				write(STDERR_FILENO, ": command not found\n", 20);
				cleanup_and_exit();

				exit(127);
			}
			execve(path, cmd->args, env_list_to_array(global->env_list));
			cleanup_and_exit();

			perror("execve");
			exit(127);
		}
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status))
		{
			int signal_num = WTERMSIG(status);
			if (signal_num == SIGINT)
				exit_status = 130;
			else if (signal_num == SIGQUIT)
				exit_status = 131;
			else
				exit_status = 128 + signal_num;
		}
		else
			exit_status = WEXITSTATUS(status);
		return (exit_status);
	}
	return (1);
}