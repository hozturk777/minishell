/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 15:27:19 by huozturk          #+#    #+#             */
/*   Updated: 2024/11/09 15:38:30 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

static char	**ft_dofree(char **s)
{
	int	i;

	i = 0;
	while (s[i])
		free(s[i++]);
	free(s);
	return (NULL);
}

static int	ft_wordcount(const char *str, int c)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*str)
	{
		if (*str != c && in_word == 0)
		{
			in_word = 1;
			count++;
		}
		else if (*str == c)
			in_word = 0;
		str++;
	}
	return (count);
}

static char	*ft_worddup(const char *s, int start, int len)
{
	char	*word;
	int		i;

	word = (char *)ft_calloc((len - start + 1), sizeof(char));
	if (!word)
		return (NULL);
	i = 0;
	while (start < len)
		word[i++] = s[start++];
	return (word);
}

static void	*ft_wordcatch(const char *s, int *start2, int *index2, char c)
{
	while (*(s + *index2) != '\0' && *(s + *index2) == c)
		(*index2)++;
	*start2 = *index2;
	while (*(s + *index2) != '\0' && *(s + *index2) != c)
		(*index2)++;
	return (NULL);
}

char	**ft_split(char const *s, char c)
{
	char		**str;
	int			start;
	int			j;
	int			index;

	if (!s)
		return (NULL);
	j = 0;
	index = 0;
	str = (char **)ft_calloc((ft_wordcount(s, c) + 1), sizeof(char *));
	if (!str)
		return (NULL);
	while (*(s + index) != '\0')
	{
		ft_wordcatch(s, &start, &index, c);
		if (index > start)
		{
			str[j] = ft_worddup(s, start, index);
			if (!str[j])
				return (ft_dofree(str));
			j++;
		}
	}
	return (str);
}
