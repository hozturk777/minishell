/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_welcome.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:56:27 by hasivaci          #+#    #+#             */
/*   Updated: 2025/10/05 20:56:29 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"
#include <stdio.h>
#include <unistd.h>

static	void	print_welcome(void)
{
	printf("|       ||   | |  _    ||   |");
	usleep(20000);
	printf(" |_____  ||       ||    ___||   |___ |   |___\n");
	usleep(20000);
	printf("| ||_|| ||   | | | |   ||");
	usleep(20000);
	printf("   |  _____| ||   _   ||   |___ |       ||       |\n");
	usleep(20000);
	printf("|_|   |_||___| |_|  |__||___|");
	usleep(20000);
	printf(" |_______||__| |__||_______||_______||_______|\n" RESET);
	usleep(20000);
	printf(BLACK "\nCREATED BY:\n");
	usleep(20000);
	printf("\t\t _____________________");
	usleep(20000);
	printf("\n\t\t/\t\t      \\\n\t       ");
	usleep(20000);
	printf("|  hasivaci & huozturk  |\n");
	usleep(20000);
	printf("\t\t\\_____________________/\n");
	usleep(20000);
	printf("\n\n" RESET);
	usleep(20000);
}

void	print_welcome_advanced(void)
{
	printf(GREEN "\033[H\033[J");
	usleep(20000);
	printf(" __   __  ___   __    _  ___   _______");
	usleep(20000);
	printf("  __   __  _______  ___      ___ \n");
	usleep(20000);
	printf("|  |_|  ||   | |  |  | ||   | |       |");
	usleep(20000);
	printf("|  | |  ||       ||   |    |   |  \n");
	usleep(20000);
	printf("|       ||   | |   |_| ||   |");
	usleep(20000);
	printf(" |  _____||  |_|  ||    ___||   |    |   |    \n");
	usleep(20000);
	printf("|       ||   | |       ||   | |");
	usleep(20000);
	printf(" |_____ |       ||   |___ |   |    |   |    \n");
	usleep(20000);
	print_welcome();
}
