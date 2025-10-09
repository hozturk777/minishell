/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/18 16:17:04 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/19 21:30:45 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_strdup(const char *s1)
{
	int		slen;
	int		i;
	char	*str;

	if (!s1)
		return (NULL);
	slen = ft_strlen(s1);
	i = 0;
	str = (char *)halloc(slen + 1);
	if (!str)
	{
		clear_garbage();
		exit(1);
	}
	while (s1[i] != '\0')
	{
		str[i] = s1[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}
