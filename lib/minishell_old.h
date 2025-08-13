/* ***************************************************************************************** */
/*                                                                                           */
/*                                                                       :::      ::::::::   */
/*   minishell.h                                                       :+:      :+:    :+:   */
/*                                                                   +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42kocaeli.com.tr>              +#+  +:+       +#+        */
/*                                                               +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 16:08:40 by huozturk                         #+#    #+#             */
/*   Updated: 2025/08/12 21:20:35 by huozturk                        ###   ########.tr       */
/*                                                                                           */
/* ***************************************************************************************** */


#ifndef MINISHELL_H
# define MINISHELL_H

# define PROMPT "minishell$ "
# define GREEN "\033[1;92m"
# define RESET "\033[0m"

# include "token_enum.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>

// Token yapısı
typedef struct s_token
{
    t_token_type	type;        // Token'ın türü
    char			*value;      // Token'ın değeri (string) | "echo hello" → value = "echo" → value = "hello" |
    int				len;         // Token'ın uzunluğu | Optimizasyon için |
    struct s_token	*next;       // Sonraki token'a pointer
}	t_token;

// Lexer yapısı
// typedef struct s_lexer
// {
//     char	*input;			      // Analiz edilen string
//     int		input_pos;            // Şu anki pozisyon
//     int		input_len;            // Input'un toplam uzunluğu
//     char	input_current_char;   // Şu anki karakter
// }	t_lexer;

// typedef struct s_list
// {
// 	void			*content;
// 	struct s_list	*next;
// 	struct s_list	*prev;
// }					t_list;
// Lexer yapısı - EKSİK OLAN KISIM!
typedef struct s_lexer
{
    char	*input;
    int		pos;
    int		len;
    char	current_char;
}	t_lexer;

// Fonksiyon prototipler
t_token		*tokenize(char *input);
t_token		*create_token(t_token_type type, char *value);
void		free_tokens(t_token *tokens);
t_lexer		*init_lexer(char *input);
void		free_lexer(t_lexer *lexer);
void		advance(t_lexer *lexer);
void		skip_whitespace(t_lexer *lexer);
void		print_tokens(t_token *tokens);
void		print_welcome();



#endif
