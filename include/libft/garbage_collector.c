/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 17:39:25 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/19 21:31:10 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// mallloc ve strdup olan heryer galloc kontrol edilcecek;
// galloc hata durumda herşeyi freelicek

// halloc içinde heryere galloc yazıp galloc içinde hata durumları ve hata çıktıları kontorl edilirse 2 satır if bloğuna greek olmadan heryeri buradan freeleyip çıkarız;
#include "libft.h"
#include <stdlib.h>

static void	*add_garbage(void *adress);

static void	*get_garbage(void)
{
	static t_list	*garbage;

	return (&garbage);
}
void	*halloc(size_t size)
{
	void	*adress;

	adress = add_garbage(malloc(size));
	// if (!adress)
	// {
	//     burada hata var yazıverecek clear garbage a gidip hepsini freleyip çıkıcak ;
	//     exit
	//      exit status 1;
	// }
	return (adress);
}

static void	*add_garbage(void *adress)
{
	t_list	**collector;
	t_list	*tmp;

	if (!adress)
		return (0);
	// if (!GARBAGE_COLLECTOR) .h daki yeri 0 yaparsam çalışmayacak ve leaklere bakabilecem
	// {
	//     return(adress);
	// }
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
	t_list **collector;
	t_list *total;
	t_list *tmp;

	collector = get_garbage();
	if (!collector || !*collector)
		return;
		
	total = *collector;
	while (total)
	{
		tmp = total->next;
		if (total->content)
		{
			free(total->content);
			total->content = NULL;  // Güvenlik için NULL yap
		}
		free(total);
		total = tmp;
	}
	*collector = NULL;  // ÖNEMLİ: Collector'ı NULL yap
}