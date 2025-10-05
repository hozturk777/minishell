/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:25 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/30 12:38:54 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

static void	*get_garbage(void)
{
	static t_list	*garbage;

	return (&garbage);
}

void	*halloc(size_t size)
{
	void	*adress;

	adress = add_garbage(malloc(size));
	return (adress);
}

void	*add_garbage(void *adress)
{
	t_list	**collector;
	t_list	*tmp;

	if (!adress)
		return (0);
	collector = get_garbage();
	tmp = malloc(sizeof(t_list));
	if (!tmp)
		return (0);
	tmp->next = NULL;
	tmp->content = adress;
	if (*collector)
	{
		tmp->next = *collector;
		*collector = tmp;
	}
	else
		*collector = tmp;
	return (tmp->content);
}

void	clear_garbage(void)
{
	t_list	**collector;
	t_list	*total;
	t_list	*tmp;

	collector = get_garbage();
	if (!collector || !*collector)
		return ;
	total = *collector;
	while (total)
	{
		tmp = total->next;
		if (total->content)
		{
			free(total->content);
			total->content = NULL;
		}
		free(total);
		total = tmp;
	}
	*collector = NULL;
}
