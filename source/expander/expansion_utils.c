/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 19:38:17 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/29 19:39:47 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../lib/minishell.h"

int	needs_expansion(char *str)
{
	int	i;
	int	in_single_quote;

	if (!str)
		return (0);
	i = 0;
	in_single_quote = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_single_quote)
			in_single_quote = 1;
		else if (str[i] == '\'' && in_single_quote)
			in_single_quote = 0;
		else if (str[i] == '$' && !in_single_quote)
			return (1);
		i++;
	}
	return (0);
}

int	is_single_quoted_literal(char *str)
{
	if (!str)
		return (0);
	return (ft_strnstr(str, "__SINGLE_QUOTE__", ft_strlen(str)) != NULL);
}

int	is_double_quoted_literal(char *str)
{
	if (!str)
		return (0);
	return (ft_strnstr(str, "\"", ft_strlen(str)) != NULL);
}

char	*extract_single_quote_content(char *str)
{
	char	*start;
	char	*end;
	int		content_len;
	char	*result;

	if (!str)
		return (NULL);
	
	start = ft_strnstr(str, "__SINGLE_QUOTE__", ft_strlen(str));
	if (!start)
		return (ft_strdup(str));
	
	start += ft_strlen("__SINGLE_QUOTE__");
	
	end = ft_strnstr(start, "__END_SINGLE_QUOTE__", ft_strlen(start));
	if (!end)
		return (ft_strdup(start));
	
	content_len = end - start;
	result = ft_substr(start, 0, content_len);
	return (result);
}

char	*ft_strjoin_char(char const *s1, char const s2)
{
	char	*str;
	int		i;

	if (!s1 && !s2)
		return (NULL);
	i = ft_strlen(s1);
	str = halloc(i + 2);
	if (!str)
	{
        clear_garbage();
		exit(1);
	}
	ft_memcpy(str, s1, i);
	ft_memcpy(str + i, &s2, 1);
	*(str + i + 1) = '\0';
	return (str);
}
