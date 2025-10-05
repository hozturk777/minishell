/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abakirca <abakirca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:03 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/01 18:47:18 by abakirca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

int	is_builtin(char *command)
{
	if (!command)
		return (0);
	if (ft_strcmp(command, "pwd") == 0)
		return (1);
	if (ft_strcmp(command, "echo") == 0)
		return (1);
	if (ft_strcmp(command, "env") == 0)
		return (1);
	if (ft_strcmp(command, "exit") == 0)
		return (1);
	if (ft_strcmp(command, "cd") == 0)
		return (1);
	if (ft_strcmp(command, "export") == 0)
		return (1);
	if (ft_strcmp(command, "unset") == 0)
		return (1);
	return (0);
}

int	handle_quoted_export(char **args, int i,
	t_global *global, int *end_index)
{
	char	*merged_arg;

	merged_arg = merge_quoted_args(args, i, end_index, global);
	if (!merged_arg)
		return (1);
	if (is_equal_export(ft_strchr(args[i], '='), merged_arg, global))
		return (2);
	return (0);
}
