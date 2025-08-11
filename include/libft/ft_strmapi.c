/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 15:29:08 by huozturk          #+#    #+#             */
/*   Updated: 2024/11/02 18:34:31 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char		*result;
	int			slen;
	size_t		i;

	i = 0;
	slen = ft_strlen(s);
	result = (char *)malloc((slen + 1) * sizeof(char));
	if (!result)
		return (NULL);
	while (*(s + i))
	{
		*(result + i) = (*f)(i, *(s + i));
		i++;
	}
	*(result + i) = '\0';
	return (result);
}
