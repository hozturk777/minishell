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

// char	*expand_variables(char *input, t_global *global)
// {
// 	char	*result;
// 	char	*temp;
// 	int		i;

// 	if (!input)
// 		return (NULL);
// 	result = ft_strdup("");
// 	if (!result)
// 		return (NULL);
// 	i = 0;
// 	while (input[i])
// 	{
// 		if (input[i] == '$')
// 			temp = handle_dollar_expansion(input, &i, global);
// 		else
// 			temp = handle_regular_char(input, &i);
// 		result = ft_strjoin(result, temp);
// 		if (!result)
// 			return (NULL);
// 	}
// 	return (result);
// }

static char	*try_partial_matches(char *var_name, int *i, int start, t_global *global)
{
    int		len;
    int		j;
    char	*partial_name;
    char	*partial_value;

    len = ft_strlen(var_name);
    j = len - 1;
    /* Sondan başlayarak kısalt ve dene */
    while (j > 0)
    {
        partial_name = ft_substr(var_name, 0, j);
        partial_value = get_env_value(global->env_list, partial_name);
        if (partial_value)
        {
            /* Partial match bulundu! Kalan kısmı geri döndür */
            *i = start + j;
            /* free(partial_name); */
            return (ft_strdup(""));
        }
        /* free(partial_name); */
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
    /* Eğer variable bulunamadı ama uzun bir name ise, kısa versiyonları dene */
    if (!var_value && ft_strlen(var_name) > 1)
    {
        var_value = try_partial_matches(var_name, i, start, global);
        if (var_value)
        {
            /* free(var_name); */
            return (var_value);
        }
    }
    /* free(var_name); */
    if (var_value)
        return (ft_strdup(var_value));
    else
        return (ft_strdup(""));
}

char	*handle_dollar_expansion(char *input, int *i, t_global *global)
{
    // char	*var_name;
    // char	*var_value;
    // int		start;

    (*i)++;
    if (input[*i] == '?')
    {
        (*i)++;
        return (ft_itoa(global->exit_status));
    }
    /* if (input[*i] == '$') */
    /* { */
    /* 	(*i)++; */
    /* 	return (ft_itoa(getpid())); */
    /* } */
    /* Digit check for positional parameters ($1, $2, etc.) */
    if (ft_isdigit(input[*i]))
    {
        (*i)++;
        /* Positional parameters not supported in minishell, return empty */
        return (ft_strdup(""));
    }
    if (!ft_isalpha(input[*i]) && input[*i] != '_')
        return (ft_strdup("$"));
    return (process_variable_name(input, i, global));
}

// char	*handle_dollar_expansion(char *input, int *i, t_global *global)
// {
// 	char	*var_name;
// 	char	*var_value;
// 	int		start;

// 	(*i)++;
// 	if (input[*i] == '?')
// 	{
// 		(*i)++;
// 		return (ft_itoa(global->exit_status));
// 	}
// 	// if (input[*i] == '$')
// 	// {
// 	// 	(*i)++;
// 	// 	return (ft_itoa(getpid()));
// 	// }
	
// 	// Digit check for positional parameters ($1, $2, etc.)
// 	if (ft_isdigit(input[*i]))
// 	{
// 		(*i)++; // Skip the digit
// 		// Positional parameters not supported in minishell, return empty
// 		return (ft_strdup(""));
// 	}
	
// 	if (!ft_isalpha(input[*i]) && input[*i] != '_')
// 		return (ft_strdup("$"));
// 	start = *i;
// 	while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
// 		(*i)++;
// 	var_name = ft_substr(input, start, *i - start);
// 	var_value = get_env_value(global->env_list, var_name);
// 	// Eğer variable bulunamadı ama uzun bir name ise, kısa versiyonları dene
// 	if (!var_value && ft_strlen(var_name) > 1) // Buraya bakılacak!!!!
// 	{
// 		int len = ft_strlen(var_name);
// 		int j;
		
// 		// Sondan başlayarak kısalt ve dene
// 		for (j = len - 1; j > 0; j--)
// 		{
// 			char *partial_name = ft_substr(var_name, 0, j);
// 			char *partial_value = get_env_value(global->env_list, partial_name);
			
// 			if (partial_value)
// 			{
// 				// Partial match bulundu! Kalan kısmı geri döndür
// 				*i = start + j; // Position'ı ayarla
// 				// free(var_name);
// 				// free(partial_name);
// 				return (ft_strdup(""));
// 			}
// 			// free(partial_name);
// 		}
// 	}
// 	if (var_value)
// 		return(ft_strdup(var_value));
// 	else
// 		return(ft_strdup(""));
// 	// free(var_name);
// 	// return (var_value ? ft_strdup(var_value) : ft_strdup(""));
// }

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

// char	*join_and_free(char *s1, char *s2) // KAPANACAK
// {
// 	char	*result;

// 	if (!s1 || !s2)
// 	{
// 		if (s1)
// 			free(s1);
// 		if (s2)
// 			free(s2);
// 		return (NULL);
// 	}
// 	result = ft_strjoin(s1, s2);
// 	// free(s1);
// 	// free(s2);
// 	return (result);
// }
