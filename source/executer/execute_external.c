/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_external.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 20:52:51 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 12:44:31 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// int	execute_external_command(t_command *cmd, t_global *global)
// {
// 	pid_t	pid;
// 	int		status;
// 	char	*path;

// 	path = find_command_path(cmd->args[0], global->env_list); // Path içerisinde command arıyor bulduğu path'in yanına "/cmd->args[0]" ekliyor

// 	if (!path) // Path içerisinde command bulunamazsa hata
// 	{
// 		printf("minishell: %s: command not found\n", cmd->args[0]);
// 		return (127);
// 	}
// 	pid = fork(); // child process açıyor ve child process kendi pid numarasını 0 olarak biliyor
// 	if (pid == 0)
// 	{
// 		// Child process - sinyalleri default davranışa çevir

// 		setup_child_signals();

// 		global->in_child = 1;
// 		setup_redirections(cmd); // Öğrenilecek (dup ve dup2)
// 		execve(path, cmd->args, env_list_to_array(global->env_list)); // Öğrenilecek!
// 		perror("execve");
// 		cleanup_and_exit();
// 		exit(127);
// 	}
// 	else if (pid > 0)
// 	{
// 		waitpid(pid, &status, 0); // Burada status pid numaralı process'in döndürdüğü exit status
// 		// free(path);
		
// 		// Signal ile sonlandı mı kontrol et
// 		if (WIFSIGNALED(status))
// 		{
// 			int signal_num = WTERMSIG(status);
// 			if (signal_num == SIGINT)
// 				return (130); // 128 + SIGINT
// 			else if (signal_num == SIGQUIT)
// 				return (131); // 128 + SIGQUIT
// 			else
// 				return (128 + signal_num);
// 		}
// 		//clear_garbage();
// 		return (WEXITSTATUS(status));
// 	}
// 	else
// 	{
// 		perror("fork");
// 		free(path);
// 		clear_garbage();
// 		return (1);
// 	}
// }


static void	handle_external_child_process(t_command *cmd, t_global *global, char *path)
{
	/* Setup child signal handlers */
    setup_child_signals();
    global->in_child = 1;
    /* Setup redirections before execution */
    setup_redirections(cmd);
    /* Execute the external command */
    execve(path, cmd->args, env_list_to_array(global->env_list));
    /* execve failed - cleanup and exit */
    perror("execve");
    cleanup_and_exit();
    exit(127);
}

static int	wait_for_external_process(pid_t pid, char *path)
{
	int	status;
	(void) path;
    /* Free path before waiting */

    /* Wait for external process to complete */
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
int	execute_external_command(t_command *cmd, t_global *global)
{
	char	*path;
	pid_t	pid;

	/* Find the command path in system PATH */
	path = find_command_path(cmd->args[0], global->env_list);
	if (!path)
	{
		/* Command not found in PATH */
		printf("minishell: %s: command not found\n", cmd->args[0]);
		return (127);
	}
	/* Fork process for external command execution */
	pid = fork();
	if (pid == 0)
	{
		/* Child process executes the external command */
		handle_external_child_process(cmd, global, path);
		exit (127);
	}
	else if (pid > 0)
	{
		/* Parent process waits for child completion */
		return (wait_for_external_process(pid, path));
	}
	else
	{
		/* Fork failed - cleanup and return error */
		perror("fork");
		free(path);
		clear_garbage();
		return (1);
	}
}
