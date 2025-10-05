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
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

static int	execute_single_command(t_command *cmd, t_global *global);

int	execute_commands(t_command *commands, t_global *global)
{
	if (!commands)
		return (0);
	if (commands->next)
		return (execute_pipeline(commands, global));
	else
		return (execute_single_command(commands, global));
}

static int	execute_single_command(t_command *cmd, t_global *global)
{
	int	originals[2];

	if ((!cmd || !cmd->args || !cmd->args[0]) && cmd->redirections)
	{
		global->in_child = 2;
		execute_redirect_command(cmd, global, NULL);
		return (1);
	}
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	originals[0] = dup(STDIN_FILENO);
	originals[1] = dup(STDOUT_FILENO);
	if (is_builtin(cmd->args[0]))
	{
		if (!ft_strcmp(cmd->args[0], "exit"))
		{
			close(originals[0]);
			close(originals[1]);
		}
		return (execute_builtin(cmd, global, originals, 1));
	}
	close(originals[0]);
	close(originals[1]);
	return (execute_external_command(cmd, global));
}

static void	handle_redirect_child_process(t_command *cmd)
{
	setup_child_signals();
	if (setup_redirections(cmd))
	{
		cleanup_and_exit();
		exit(2);
	}
	cleanup_and_exit();
	exit(0);
}

int	wait_for_redirect_process(pid_t pid)
{
	int	status;
	int	signal_num;

	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		signal_num = WTERMSIG(status);
		if (signal_num == SIGINT)
			return (130);
		else if (signal_num == SIGQUIT)
			return (131);
		else
			return (128 + signal_num);
	}
	return (WEXITSTATUS(status));
}

void	execute_redirect_command(t_command *cmd, t_global *global, int *origin)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (origin)
		{
			close(origin[0]);
			close(origin[1]);
		}
		handle_redirect_child_process(cmd);
	}
	else if (pid > 0)
	{
		global->in_child = 0;
		global->exit_status = wait_for_redirect_process(pid);
	}
}
