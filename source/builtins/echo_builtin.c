/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 20:29:23 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/04 18:35:49 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>

static void process_escape_sequences(char *str)
{
    int i;
    int j;

    i = 0;
    j = 0;
    while (str[i])
    {
        if (str[i] == '\\' && str[i + 1])
        {
            if (str[i + 1] == '\\')
            {
                str[i] = '\\';
                str[j] = str[i];

            }
            else
            {
                str[j] = str[i+1];
            }
            i++;
        }       
        else
            str[j] = str[i];
        j++;
        i++;
    }
    str[j] = '\0';
}

int is_n_flag(char *str)
{
    int i;

    i = 0;
    if (!str || str[0] != '-' || str[1] == '\0')
        return (0);
    i++;
    while (str[i])
    {
        if (str[i] != 'n')
            return(0);
        i++;
    }
    return(1);
}

int builtin_echo(char **args)
{
    int     i;
    int     j;
    int     b;
    int     newline;
    int     enable_escape;
    char    *processed_arg;

    i = 1;
    j = 1;
    b = 0;
    newline = 1;
    enable_escape = 0;
    


	while (args[i] && is_n_flag(args[i]))
	{
		newline = 0;
		i++;
		if (args[i] && !ft_strcmp(args[i], " "))
			i++;
	}

    j = i;
    while (args[j])
    {
        b = 0;
        while (args[j][b])
        {
            if (args[j][b] == '\\')
                enable_escape = 1;
            b++;
        }
        j++;
    }
    
    j = i;
    while (args[j])
    {
        processed_arg = args[j];
        if (processed_arg)
        {
            if (enable_escape)
            {
                process_escape_sequences(processed_arg);
                enable_escape = 0;
            }
            while (processed_arg && !ft_strcmp(processed_arg, " "))
            {
                if (!ft_strcmp(args[j + 1], " "))
                {
                    j++;
                    processed_arg = args[j];
                }
                else
                {
                    break;
                }
            }
            printf("%s", processed_arg);
        }
        j++;
    }
    if (newline)
        printf("\n");
    return (0);
}

