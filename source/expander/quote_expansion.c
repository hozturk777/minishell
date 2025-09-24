/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 12:00:00 by hsyn              #+#    #+#             */
/*   Updated: 2025/09/20 18:15:42 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

char	*expand_with_quotes(char *input, t_global *global)
{
	char	*result;
	char	*temp;
	int		i;
	int		quote_state[2];

	result = ft_strdup(""); // Buna check eklenecek!
	i = 0;
	quote_state[0] = 0;  // single quote
	quote_state[1] = 0;  // double quote
	while (input[i])
	{
		update_quote_state(input[i], quote_state);
		if (input[i] == '$' && !quote_state[0])
			temp = handle_dollar_expansion(input, &i, global);
		else if (input[i] == '\'' && quote_state[1]) // Single quote inside double quote - keep as literal
			temp = handle_regular_char(input, &i);
		else if (input[i] == '"' && quote_state[0]) // Double quote inside single quote - keep as literal  
			temp = handle_regular_char(input, &i);
		else if (input[i] != '\'' && input[i] != '"')
			temp = handle_regular_char(input, &i);
		else
		{
			i++;
			continue ;
		}
		result = ft_strjoin(result, temp);
		if (!result)
			return (NULL);
	}
	return (result);
}

void	update_quote_state(char c, int *quote_state)
{
	if (c == '\'' && !quote_state[1])
		quote_state[0] = !quote_state[0];
	else if (c == '"' && !quote_state[0])
		quote_state[1] = !quote_state[1];
}

char	*remove_outer_quotes(char *input)
{
	int		len;
	char	*result;

	if (!input)
		return (NULL);
	len = ft_strlen(input);
	if (len < 2)
		return (ft_strdup(input));
	if ((input[0] == '\'' && input[len - 1] == '\'')
		|| (input[0] == '"' && input[len - 1] == '"')) // tek tırnaktan sonra çift geliyorsa çifti al komutu çalıştırma
	{
		result = ft_substr(input, 1, len - 2);
		return (result);
	}
	return (ft_strdup(input));
}

int	needs_expansion(char *str) // '$' var ise expanded için 1 dönecek
{
	int	i;
	int	in_single_quote;

	if (!str)
		return (0);
	i = 0;
	in_single_quote = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_single_quote) // Tek tırnak açıldığında 1
			in_single_quote = 1;
		else if (str[i] == '\'' && in_single_quote) // Tek tırnak kapandığında 0
			in_single_quote = 0;
		else if (str[i] == '$' && !in_single_quote) // Eğer tek tırnak açık kalmadıysa ve $ geldiyse expanded işlemi için 1 döner
			return (1);
		i++;
	}
	return (0);
}

void	expand_command_args(t_command *cmd, t_global *global)
{
	int		i;
	char	*expanded;
	char	*clean_str;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		// Check if this is a single-quoted token (no expansion needed)
		if (is_single_quoted_literal(cmd->args[i]))
		{
			// Remove the marker and keep content as-is (no expansion)
			clean_str = extract_single_quote_content(cmd->args[i]);
			// free(cmd->args[i]);
			cmd->args[i] = clean_str;
		}
		else if (needs_expansion(cmd->args[i]))
		{
			expanded = expand_with_quotes(cmd->args[i], global);
			if (expanded)
			{
				// free(cmd->args[i]);
				cmd->args[i] = expanded;
			}
		}
		else
		{
			expanded = remove_outer_quotes(cmd->args[i]);
			if (expanded)
			{
				// free(cmd->args[i]);
				cmd->args[i] = expanded;
			}
		}
		i++;
	}
	filter_empty_args(cmd);
}

void	filter_empty_args(t_command *cmd)
{
	int		i;
	int		j;
	char	**new_args;
	int		count;

	if (!cmd || !cmd->args)
		return ;
	count = count_non_empty_args(cmd->args);
	new_args = halloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return ;
	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] != '\0')
		{
			new_args[j] = cmd->args[i];
			cmd->args[i] = NULL;
			j++;
		}
		// else
		// 	free(cmd->args[i]);
		i++;
	}
	new_args[j] = NULL;
	// free(cmd->args);
	cmd->args = new_args;
}

int	count_non_empty_args(char **args)
{
	int	i;
	int	count;

	if (!args)
		return (0);
	i = 0;
	count = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	return (count);
}

int	is_single_quoted_literal(char *str)
{
	// Check if string has single quote marker
	if (!str)
		return (0);
	return (ft_strnstr(str, "__SINGLE_QUOTE__", ft_strlen(str)) != NULL);
}

char	*extract_single_quote_content(char *str)
{
	char	*start;
	char	*end;
	int		content_len;
	char	*result;

	if (!str)
		return (NULL);
	
	// Find start marker
	start = ft_strnstr(str, "__SINGLE_QUOTE__", ft_strlen(str));
	if (!start)
		return (ft_strdup(str));
	
	start += ft_strlen("__SINGLE_QUOTE__");
	
	// Find end marker
	end = ft_strnstr(start, "__END_SINGLE_QUOTE__", ft_strlen(start));
	if (!end)
		return (ft_strdup(start));
	
	content_len = end - start;
	result = ft_substr(start, 0, content_len);
	return (result);
}
