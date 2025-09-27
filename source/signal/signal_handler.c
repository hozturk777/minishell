/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 15:30:00 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/27 20:14:42 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// Global değişken - shell'in ana durumunu tutar
// extern t_global	*g_global;

/**
 * SIGQUIT (Ctrl+\) sinyal handler'ı
 * - Interactive modda: Hiçbir şey yapma (ignore)
 * - Child process'te: Core dump ile sonlandır
 */
void	sigquit_handler(int sig)
{
	t_global	*g_global;

	(void)sig;
	g_global = get_global();
	if (g_global && g_global->in_child)
	{
		// Child process'te - default davranış (core dump)
		exit(131); // 128 + SIGQUIT signal number (3)
	}
	// Parent process'te hiçbir şey yapma (ignore)
}

/**
 * Interactive shell için sinyalleri ayarla
 * - SIGINT: Yeni prompt göster
 * - SIGQUIT: Ignore et
 */
void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// SIGINT (Ctrl+C) ayarlama
	sa_int.sa_handler = sigint_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);
	// SIGQUIT (Ctrl+\) ayarlama
	sa_quit.sa_handler = SIG_IGN; // Ignore et
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * Child process'ler için sinyalleri default davranışa çevir
 */// Araştırılacak!!
void	setup_child_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// SIGINT (Ctrl+C) ayarlama
	sa_int.sa_handler = sigint_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);
	// SIGQUIT (Ctrl+\) ayarlama
	sa_quit.sa_handler = SIG_IGN; // Ignore et
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * Sinyalleri varsayılan davranışa geri yükle
 */
void	restore_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * EOF (Ctrl+D) işlemesi
 * readline NULL döndürdüğünde çağrılır
 */
void	handle_eof(void)
{
	t_global	*g_global;

	g_global = get_global();
	printf("exit\n");
	// sigint_handler_child_cleanup(g_global->commands);
	if (g_global)
	{
		g_global->should_exit = 1;
		g_global->exit_status = 0;
	}
}
