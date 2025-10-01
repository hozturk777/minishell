/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abakirca <abakirca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:47:26 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/01 18:32:10 by abakirca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

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
	int originals[2];
	originals[0] = dup(STDIN_FILENO);
	originals[1] = dup(STDOUT_FILENO);
	if (is_builtin(cmd->args[0]))
		return (execute_builtin(cmd, global, originals));
	close(originals[0]);
	close(originals[1]);
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
