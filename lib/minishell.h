/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/22 05:32:34 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ************************************************************************** */
/*                               TANIMLAMALAR                                 */
/* ************************************************************************** */

# define PROMPT "minishell$ "		/* Terminal prompt metni */
# define GREEN "\033[1;92m"			/* Yeşil renk için ANSI kod */
# define RESET "\033[0m"			/* Renk sıfırlama ANSI kodu */

/* ************************************************************************** */
/*                               DAHIL EDILEN DOSYALAR                       */
/* ************************************************************************** */

// # include "token_enum.h"
# include "../include/libft/libft.h"	/* Libft fonksiyonları */
# include <stdio.h>						/* printf, perror fonksiyonları */
# include <stdlib.h>					/* malloc, free, exit fonksiyonları */
# include <unistd.h>					/* write, read, access, fork fonksiyonları */
# include <readline/readline.h>			/* readline fonksiyonu */
# include <readline/history.h>			/* add_history, rl_clear_history fonksiyonları */
# include <signal.h>					/* signal, sigaction, kill fonksiyonları */
# include <sys/wait.h>					/* wait, waitpid, wait3, wait4 fonksiyonları */
# include <fcntl.h>						/* open, close fonksiyonları */
# include <errno.h>						/* Hata yönetimi */
# include <string.h>					/* String manipülasyon fonksiyonları */

/* ************************************************************************** */
/*                               ENUM'LAR                                     */
/* ************************************************************************** */

typedef struct s_global t_global;

// Leksikal analiz için token türleri
typedef enum e_token_types
{
	T_WORD,				/* Normal kelimeler: komutlar, argümanlar, dosya adları */
	T_PIPE,				/* Pipe operatörü: | */
	T_REDIRECT_IN,		/* Giriş yönlendirmesi: < */
	T_REDIRECT_OUT,		/* Çıkış yönlendirmesi: > */
	T_APPEND,			/* Ekleme yönlendirmesi: >> */
	T_HEREDOC,			/* Here document: << */
	T_SINGLE_QUOTE,		/* Tek tırnakla çevrili metin: 'metin' */
	T_DOUBLE_QUOTE,		/* Çift tırnakla çevrili metin: "metin" */
	T_ENV_VAR,			/* Çevre değişkeni: $VAR */
	T_WHITESPACE,		/* Boşluk karakteri (genellikle atlanır) */
	T_EOF				/* Girdi sonu */
}	t_token_types;

/* ************************************************************************** */
/*                               YAPILAR                                      */
/* ************************************************************************** */

// Gelişmiş token yapısı
typedef struct s_token_new
{
	t_token_types	type;			/* Token türü (T_WORD, T_PIPE, vb.) */
	char			*value;			/* Token string değeri */
	int				len;			/* Token değerinin uzunluğu */
	int				quote_type;		/* Tırnak türü: 0=yok, 1=tek, 2=çift */
	int				expanded;		/* Değişken genişletmesi yapıldı mı (1=evet, 0=hayır) */
}	t_token_new;

// Çevre değişkeni yapısı
typedef struct s_env
{
	char			*key;			/* Değişken adı (örn: "HOME", "PATH") */
	char			*value;			/* Değişken değeri (örn: "/Users/kullanici") */
	struct s_env	*next;			/* Bağlı listedeki sonraki çevre değişkeni */
}	t_env;

// Yönlendirme yapısı
typedef struct s_redirect
{
	t_token_types		type;		/* Yönlendirme türü (T_REDIRECT_IN, T_REDIRECT_OUT, vb.) */
	char				*filename;	/* Yönlendirme için hedef dosya adı */
	int					fd;			/* Yönlendirme için dosya tanımlayıcı */
	struct s_redirect	*next;		/* Bağlı listedeki sonraki yönlendirme */
}	t_redirect;

// Komut yapısı
typedef struct s_command
{
	char				**args;		/* Komut ve argümanlar dizisi (["ls", "-l", NULL]) */
	t_list				*redirections;	/* Bu komut için yönlendirmeler listesi */
	int					pipe_fd[2];	/* Pipe dosya tanımlayıcıları [okuma_ucu, yazma_ucu] */
	pid_t				pid;		/* Komut çalıştırıldığında süreç ID'si */
	struct s_command	*next;		/* Pipeline'daki sonraki komut */
}	t_command;


// Lexer yapısı (gelişmiş)
typedef struct s_lexer_new
{
	char		*input;			/* Tokenize edilecek girdi metni */
	int			pos;			/* Girdi metnindeki mevcut pozisyon */
	int			len;			/* Girdi metninin uzunluğu */
	char		current_char;	/* İşlenmekte olan mevcut karakter */
	t_global	*global;		/* Global duruma referans */
}	t_lexer_new;

// Global durum yapısı
typedef struct s_global
{
	t_list		*tokens;		/* Ayrıştırılan token'lar listesi */
	t_command	*commands;		/* Ayrıştırılan komutlar listesi */
	t_env		*env_list;		/* Çevre değişkenleri bağlı listesi */
	int			exit_status;	/* Son komutun çıkış durumu */
	int			pipe_count;		/* Mevcut komuttaki pipe sayısı */
	int			heredoc_count;	/* Mevcut komuttaki heredoc sayısı */
	char		*input_line;	/* İşlenmekte olan mevcut girdi satırı */
	int			interactive;	/* Etkileşimli mod bayrağı (1=evet, 0=hayır) */
}	t_global;

/* ************************************************************************** */
/*                            FONKSIYON PROTOTIPLERI                         */
/* ************************************************************************** */

// ========== LEXER VE TOKENIZER FONKSIYONLARI ==========
t_list			*tokenize_advanced(char *input, t_global *global);		/* Ana tokenize fonksiyonu */
t_token_new		*create_token_advanced(t_token_types type, char *value);	/* Yeni token oluştur */
void			free_token_advanced(void *token);						/* Tek token serbest bırak */
void			free_tokens_advanced(t_list **tokens);					/* Token listesini serbest bırak */
t_lexer_new		*init_lexer_advanced(char *input, t_global *global);	/* Lexer başlat */
void			free_lexer_advanced(t_lexer_new *lexer);				/* Lexer serbest bırak */
void			advance_lexer(t_lexer_new *lexer);						/* Sonraki karaktere geç */

// ========== ÇEVRE DEĞIŞKENI FONKSIYONLARI ==========
t_env			*create_env_node(char *key, char *value);	/* Çevre değişkeni düğümü oluştur */
void			add_env_node(t_env **env_list, t_env *new_node);		/* Çevre düğümünü listeye ekle */
t_env			*find_env_node(t_env *env_list, char *key);				/* Anahtar ile çevre değişkeni bul */
void			free_env_list(t_env *env_list);							/* Çevre listesini serbest bırak */
t_env			*init_env_from_envp(char **envp);						/* Envp'den çevre başlat */

// ========== TOKEN İŞLEYİCİ FONKSIYONLARI ==========
t_token_new		*handle_pipe_advanced(t_lexer_new *lexer);				/* Pipe token'ını işle */
t_token_new		*handle_redirect_advanced(t_lexer_new *lexer);			/* Yönlendirme token'larını işle */
t_token_new		*handle_word_advanced(t_lexer_new *lexer);				/* Kelime token'larını işle */
t_token_new		*handle_quotes_advanced(t_lexer_new *lexer);			/* Tırnaklı metinleri işle */

// ========== KOMUT FONKSIYONLARI ==========
t_command		*create_command(void);									/* Yeni komut yapısı oluştur */
void			free_command(t_command *cmd);							/* Tek komut serbest bırak */
void			free_commands(t_list **commands);						/* Komut listesini serbest bırak */
t_command		*parse_tokens_to_commands(t_list *tokens, t_global *global);	/* Token'ları komutlara ayrıştır */

// ========== GLOBAL DURUM FONKSIYONLARI ==========
t_global		*init_global(char **envp);								/* Global durumu başlat */
void			free_global(t_global *global);							/* Global durumu serbest bırak */
void			update_exit_status(t_global *global, int status);		/* Çıkış durumunu güncelle */

// ========== YARDIMCI FONKSIYONLAR ==========
void			print_tokens_advanced(t_list *tokens);					/* Token'ları debug yazdır */
void			print_commands(t_list *commands);						/* Komutları debug yazdır */
char			*expand_env_var(char *var_name, t_global *global);		/* Çevre değişkenini genişlet */
int				is_special_char(char c);								/* Karakter özel mi kontrol et */
void			debug_print(char *msg);									/* Debug mesajı yazdır */

// ========== PARSER FONKSIYONLARI ==========
t_command		*parse_single_command(t_list **token_node, t_global *global);
void			parse_redirection(t_command *cmd, t_list **token_node, t_global *global);
int				is_command_start(t_list *token_node);
int				is_pipe_token(t_list *token_node);
int				is_redirect_token(t_list *token_node);
int				is_word_token(t_list *token_node);
void			collect_command_arg(t_list **args_list, t_list *token_node);
char			**convert_list_to_array(t_list *args_list);
void			append_command_to_chain(t_command *head, t_command *new_cmd);
void			add_redirect_to_command(t_command *cmd, t_redirect *redirect);

// ========== DEBUG FONKSIYONLARI ==========
void			print_commands_debug(t_command *commands);				/* Komutları debug yazdır */
void			free_commands_list(t_command *commands);				/* Komut listesini serbest bırak */

#endif
