/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 12:26:39 by huozturk          #+#    #+#             */
/*   Updated: 2024/11/07 13:03:58 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	ft_lennum(int n, int sign)
{
	int	i;

	i = 0;
	if (n == 0)
		i = 1;
	while (n != 0)
	{
		i++;
		n /= 10;
	}
	if (sign < 0)
		i++;
	return (i);
}

char	*ft_itoa(int n)
{
	char				*result;
	unsigned int		num;
	int					sign;
	int					lennum;

	num = n;
	sign = 1;
	if (n < 0)
		sign = -1;
	num *= sign;
	lennum = ft_lennum(n, sign);
	result = (char *)malloc((lennum + 1) * sizeof(char));
	if (!result)
		return (NULL);
	*(result + lennum) = '\0';
	while (--lennum >= 0)
	{
		*(result + lennum) = (num % 10) + 48;
		num /= 10;
	}
	if (sign == -1)
		*(result + 0) = '-';
	return (result);
}
