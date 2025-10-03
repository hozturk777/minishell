/* ***********************************************************		var_value = get_env_value(global->env_list, var_name);************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variable_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 12:00:00 by hsyn              #+#    #+#             */
/*   Updated: 2025/08/22 12:00:00 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

static char	*try_partial_matches(char *var_name, int *i, int start, t_global *global)
{
    int		len;
    int		j;
    char	*partial_name;
    char	*partial_value;

    len = ft_strlen(var_name);
    j = len - 1;
    while (j > 0)
    {
        partial_name = ft_substr(var_name, 0, j);
        partial_value = get_env_value(global->env_list, partial_name);
        if (partial_value)
        {
            *i = start + j;
            return (ft_strdup(""));
        }
        j--;
    }
    return (NULL);
}

static char	*process_variable_name(char *input, int *i, t_global *global)
{
    char	*var_name;
    char	*var_value;
    int		start;

    start = *i;
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
        (*i)++;
    var_name = ft_substr(input, start, *i - start);
    var_value = get_env_value(global->env_list, var_name);
    if (!var_value && ft_strlen(var_name) > 1)
    {
        var_value = try_partial_matches(var_name, i, start, global);
        if (var_value)
        {
            return (var_value);
        }
    }
    if (var_value)
        return (ft_strdup(var_value));
    else
        return (ft_strdup(""));
}

char	*handle_dollar_expansion(char *input, int *i, t_global *global)
{
    (*i)++;
    if (input[*i] == '?')
    {
        (*i)++;
        return (ft_itoa(global->exit_status));
    }
    if (ft_isdigit(input[*i]))
    {
        (*i)++;
        return (ft_strdup(""));
    }
    if (!ft_isalpha(input[*i]) && input[*i] != '_')
        return (ft_strdup("$"));
    return (process_variable_name(input, i, global));
}


char	*handle_regular_char(char *input, int *i)
{
	char	*result;

	result = halloc(2);
	if (!result)
		return (NULL);
	result[0] = input[*i];
	result[1] = '\0';
	(*i)++;
	return (result);
}
