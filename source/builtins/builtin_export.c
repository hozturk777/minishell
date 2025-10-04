/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:26:26 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/04 19:13:41 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>

static void handle_single_assignment(char *arg, t_global *global)
{
    char    *equal_sign;
    char    *key;
    char    *value;

    equal_sign = ft_strchr(arg, '=');
    if (equal_sign)
    {
        *equal_sign = '\0';
        key = arg;
        value = equal_sign + 1;
        set_env_var(global, key, value);
        *equal_sign = '=';
    }
    else
    {
        key = arg;
        set_env_var(global, key, NULL);
    }
}

static void handle_split_assignment(char **args, int i, t_global *global)
{
    char    *key;
    int     key_len;

    key_len = ft_strlen(args[i]) - 1;
    key = ft_substr(args[i], 0, key_len);
    if (!key)
        return ;
    set_env_var(global, key, "");
}

static int  is_valid_key_char(char *arg, char *equal_pos)
{
    int h;

    h = 0;
    while (arg[h] && (equal_pos == NULL || &arg[h] < equal_pos))
    {
        if (arg[h] == '-')
            return (0);
        h++;
    }
    return (1);
}

static int  check_token_at_index(t_global *global, int index)
{
    t_list      *current;
    t_token_new *token;
    int         count;

    if (!global || !global->tokens)
        return (0);
    current = global->tokens;
    count = 0;
    while (current && count < index)
    {
        current = current->next;
        count++;
    }
    if (current)
    {
        token = (t_token_new *)current->content;
        if (token && (token->type == T_DOUBLE_QUOTE || token->type == T_SINGLE_QUOTE))
            return (1);
    }
    return (0);
}

static char *merge_quoted_args(char **args, int start_index, int *end_index, t_global *global)
{
    char    *result;
    char    *temp;
    int     i;

    result = ft_strdup(args[start_index]);
    if (!result)
        return (NULL);
    i = start_index + 1;
    
    if (args[i] && check_token_at_index(global, i))
    {
        temp = ft_strjoin(result, args[i]);
        if (!temp)
            return (NULL);
        result = temp;
        *end_index = i;
    }
    else
    {
        *end_index = start_index;
    }
    
    return (result);
}


static int  process_export_argument(char **args, int i, t_global *global)
{
    int arg_len;

    arg_len = ft_strlen(args[i]);
    if (arg_len > 0 && args[i][arg_len - 1] == '=')
    {
        handle_split_assignment(args, i, global);
        return (i + 1);
    }
    else
    {
        handle_single_assignment(args[i], global);
        return (i + 1);
    }
}

int builtin_export(char **args, t_global *global)
{
    int     i;
    int     h;
    int     end_index;
    char    *equal_pos;
    char    *merged_arg;

    h = 0;
    if (!args[1])
    {
        print_export_env(global->env_list);
        return (0);
    }
    while (args[h])
        h++;
    // printf("args = %d\n", h);
    i = 1;
    while (args[i])
    {
        equal_pos = ft_strchr(args[i], '=');
        if (equal_pos && args[i + 1] && check_token_at_index(global, i + 1))
        {
            merged_arg = merge_quoted_args(args, i, &end_index, global);
            if (!merged_arg)
                return (1);
            equal_pos = ft_strchr(merged_arg, '=');
            if (!is_valid_key_char(merged_arg, equal_pos))
            {
				// bash: export: -c: invalid option
                printf("minishell: export: -%c: invalid option\n", merged_arg[1]);
                return (2);
            }
            handle_single_assignment(merged_arg, global);
            i = end_index + 1;
        }
        else
        {
            if (!is_valid_key_char(args[i], equal_pos))
            {
                printf("minishell: export: -%c: invalid option\n", args[i][1]);
                return (2);
            }
            i = process_export_argument(args, i, global);
        }
    }
    return (0);
}