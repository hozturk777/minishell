/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 18:02:29 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/11 18:09:37 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define PROMPT "minishell$ "
# define GREEN		"\033[1;92m"
# define RESET		"\033[0m"

#include "token_enum.h"

// Token yapısı
typedef struct s_token
{
    t_token_type	type;        // Token'ın türü
    char			*value;      // Token'ın değeri (string) | "echo hello" → value = "echo" → value = "hello" |
    int				len;         // Token'ın uzunluğu | Optimizasyon için |
    struct s_token	*next;       // Sonraki token'a pointer
}	t_token;

// Lexer yapısı
typedef struct s_lexer
{
    char	*input;			      // Analiz edilen string
    int		input_pos;            // Şu anki pozisyon
    int		input_len;            // Input'un toplam uzunluğu
    char	input_current_char;   // Şu anki karakter
}	t_lexer;

#endif
