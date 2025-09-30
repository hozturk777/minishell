/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 15:30:00 by hasivaci          #+#    #+#             */
/*   Updated: 2025/09/27 23:12:50 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

// Global değişken - shell'in ana durumunu tutar
// extern t_global	*g_global;

/**
 * SIGINT (Ctrl+C) sinyal handler'ı
 * - Interactive modda: Yeni prompt göster
 * - Child process'te: Process'i sonlandır
 */

/**
 * SIGQUIT (Ctrl+\) sinyal handler'ı  
 * - Interactive modda: Hiçbir şey yapma (ignore)
 * - Child process'te: Core dump ile sonlandır
 */
void	sigquit_handler(int sig)
{
	(void)sig;
	t_global *g_global;
	
	g_global = get_global();
	if (g_global && g_global->in_child)
	{
		// Child process'te heredoc FD'leri kapat
		cleanup_and_exit();  // FD'leri kapat ve garbage collect
		// Child process'te - default davranış (core dump)
		exit(131); // 128 + SIGQUIT signal number (3)
	}
	// Parent process'te hiçbir şey yapma (ignore)
}

/**
 * SIGPIPE sinyal handler'ı
 * - Pipeline'larda broken pipe durumunda crash engelle
 */
// void	sigpipe_handler(int sig)
// {
// 	(void)sig;
// 	// SIGPIPE'ı ignore et, sadece exit status ayarla
// 	// Bu pipeline'da normal bir durum olabilir
// }

/**
 * Interactive shell için sinyalleri ayarla
 * - SIGINT: Yeni prompt göster
 * - SIGQUIT: Ignore et
 */
void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	// struct sigaction	sa_pipe;

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

	// SIGPIPE ayarlama - pipeline'larda broken pipe ignore et
	// sa_pipe.sa_handler = sigpipe_handler;
	// sigemptyset(&sa_pipe.sa_mask);
	// sa_pipe.sa_flags = 0;
	// sigaction(SIGPIPE, &sa_pipe, NULL);
}

/**
 * Child process'ler için sinyalleri default davranışa çevir
 */
void	setup_child_signals(void) // Araştırılacak!!
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
	// signal(SIGPIPE, SIG_IGN);
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
	t_global *g_global;

	g_global = get_global();
	printf("exit\n");
	// FD cleanup main'de yapılacak
	if (g_global)
	{
		g_global->should_exit = 1;
		g_global->exit_status = 0;
	}
}
