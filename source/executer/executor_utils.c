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

int	execute_commands(t_command *commands, t_global *global)
{
	if (!commands)
		return (0);
	if (commands->next)
		return (execute_pipeline(commands, global));
	else
		return (execute_single_command(commands, global));
}

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

static void	handle_redirect_child_process(t_command *cmd, t_global *global)
{
    setup_child_signals();
    global->in_child = 1;
    if(setup_redirections(cmd))
	{
		cleanup_and_exit();
		exit(2);
	}
	cleanup_and_exit();
    exit(0);
}

static int	wait_for_redirect_process(pid_t pid)
{
	int	status;

    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status))
    {
		int signal_num = WTERMSIG(status);
        if (signal_num == SIGINT)
		return (130);
        else if (signal_num == SIGQUIT)
		return (131);
        else
		return (128 + signal_num);
    }
    return (WEXITSTATUS(status));
}

int	execute_redirect_command(t_command *cmd, t_global *global)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		handle_redirect_child_process(cmd, global);
	else if (pid > 0)
		return (wait_for_redirect_process(pid));
	return (1);
}
