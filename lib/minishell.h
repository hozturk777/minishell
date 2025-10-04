/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abakirca <abakirca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/10/01 18:29:40 by abakirca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# define PROMPT "minishell$ "
# define GREEN "\033[1;92m"
# define RESET "\033[0m"
# define MAX_HEREDOC_FDS 32

# include "../include/libft/libft.h"	/* Libft fonksiyonları */
# include <stdlib.h>					/* halloc, free, exit fonksiyonları */
# include <fcntl.h>						/* open, close fonksiyonları */

typedef struct s_global t_global;
typedef enum e_token_types
{
	T_WORD,
	T_CMD,
	T_PIPE,
	T_REDIRECT_IN,
	T_REDIRECT_OUT,
	T_APPEND,
	T_HEREDOC,
	T_SINGLE_QUOTE,
	T_DOUBLE_QUOTE,
	T_WHITESPACE,
}	t_token_types;

typedef struct s_token_new
{
	t_token_types	type;			/* Token türü (T_WORD, T_PIPE, vb.) */
	char			*value;			/* Token string değeri */
	int				len;			/* Token değerinin uzunluğu */
	int				quote_type;		/* Tırnak türü: 0=yok, 1=tek, 2=çift */
	int				expanded;		/* Değişken genişletmesi yapıldı mı (1=evet, 0=hayır) */
}	t_token_new;

typedef struct s_env
{
	char			*key;			/* Değişken adı (örn: "HOME", "PATH") */
	char			*value;			/* Değişken değeri (örn: "/Users/kullanici") */
	struct s_env	*next;			/* Bağlı listedeki sonraki çevre değişkeni */
}	t_env;

typedef struct s_redirect
{
	t_token_types		type;		/* Yönlendirme türü (T_REDIRECT_IN, T_REDIRECT_OUT, vb.) */
	char				*filename;	/* Yönlendirme için hedef dosya adı */
	int					fd;			/* Yönlendirme için dosya tanımlayıcı */
	struct s_redirect	*next;		/* Bağlı listedeki sonraki yönlendirme */
}	t_redirect;

typedef struct s_command
{
	char				**args;		/* Komut ve argümanlar dizisi (["ls", "-l", NULL]) */
	t_list				*redirections;	/* Bu komut için yönlendirmeler listesi */
	int					pipe_fd[2];	/* Pipe dosya tanımlayıcıları [okuma_ucu, yazma_ucu] */
	pid_t				pid;		/* Komut çalıştırıldığında süreç ID'si */ // LAZIM MI EMİN DEĞİLİM
	struct s_command	*next;		/* Pipeline'daki sonraki komut */
}	t_command;


// Lexer yapısı (gelişmiş)
typedef struct s_lexer_new
{
	char		*input;			/* Tokenize edilecek girdi metni */
	int			pos;			/* Girdi metnindeki mevcut pozisyon */
	int			len;			/* Girdi metninin uzunluğu */
	int			t_cmd_flag;
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
	int			heredoc_count;	/* Mevcut komuttaki heredoc sayısı */
	char		*input_line;	/* İşlenmekte olan mevcut girdi satırı */
	int			in_child;		/* Child process'te mi (1=evet, 0=hayır) */
	int			should_exit;	/* Çıkış yapılması gerekiyor mu */
	int			echo_flag;
	int			heredoc_fds[MAX_HEREDOC_FDS];	/* Açık heredoc FD'leri */
	int			heredoc_fd_count;	/* Açık heredoc FD sayısı */
}	t_global;

t_global *get_global(void);


//		SİLİNECEK
void			print_tokens_advanced(t_list *tokens);					/* Token'ları debug yazdır */
void			print_commands(t_list *commands);						/* Komutları debug yazdır */
void			debug_print(char *msg);									/* Debug mesajı yazdır */
void			print_commands_debug(t_command *commands);				/* Komutları debug yazdır */





t_list			*tokenize_advanced(char *input, t_global *global);		/* Ana tokenize fonksiyonu */
t_token_new		*create_token_advanced(t_token_types type, char *value);	/* Yeni token oluştur */
t_lexer_new		*init_lexer_advanced(char *input, t_global *global);	/* Lexer başlat */
void			advance_lexer(t_lexer_new *lexer);						/* Sonraki karaktere geç */

t_env			*create_env_node(char *key, char *value);	/* Çevre değişkeni düğümü oluştur */
void			add_env_node(t_env **env_list, t_env *new_node);		/* Çevre düğümünü listeye ekle */
t_env			*find_env_node(t_env *env_list, char *key);				/* Anahtar ile çevre değişkeni bul */
t_env			*init_env_from_envp(char **envp);						/* Envp'den çevre başlat */

t_token_new		*handle_pipe_advanced(t_lexer_new *lexer);				/* Pipe token'ını işle */
t_token_new		*handle_redirect_advanced(t_lexer_new *lexer);			/* Yönlendirme token'larını işle */
t_token_new		*handle_word_advanced(t_lexer_new *lexer);				/* Kelime token'larını işle */
t_token_new		*handle_quotes_advanced(t_lexer_new *lexer);			/* Tırnaklı metinleri işle */
t_token_new		*handle_whitespaces_advanced(t_lexer_new *lexer);

t_command		*create_command(void);									/* Yeni komut yapısı oluştur */
t_command		*parse_tokens_to_commands(t_list *tokens, t_global *global);	/* Token'ları komutlara ayrıştır */

t_global		*init_global(char **envp, t_global *global);								/* Global durumu başlat */
void			register_heredoc_fd(int fd);							/* Heredoc FD'yi kaydet */
void			close_all_heredoc_fds(void);							/* Tüm heredoc FD'leri kapat */
void			cleanup_and_exit(void);								/* Exit handler */

t_command		*parse_single_command(t_list **token_node, t_global *global);
int				is_command_start(t_list *token_node);
int				is_pipe_token(t_list *token_node);
int				is_redirect_token(t_list *token_node);
int				is_word_token(t_list *token_node);
void			collect_command_arg(t_list **args_list, t_list *token_node);
char			**convert_list_to_array(t_list *args_list);
void			append_command_to_chain(t_command *head, t_command *new_cmd);
void			add_redirect_to_command(t_command *cmd, t_redirect *redirect);

char			*handle_dollar_expansion(char *input, int *i, t_global *global);	/* $VAR genişletme */
char			*handle_regular_char(char *input, int *i);				/* Normal karakter işleme */

char			*expand_with_quotes(char *input, t_global *global);		/* Tırnakla genişletme */
char			*expand_with_heredoc(char *input, t_global *global);
char			*remove_outer_quotes(char *input);						/* Dış tırnakları kaldır */
int				needs_expansion(char *str);								/* Genişletme gerekli mi kontrol et */
void			expand_command_args(t_command *cmd, t_global *global);	/* Komut argümanlarını genişlet */
int				is_single_quoted_literal(char *str);					/* Tek tırnaklı literal mi kontrol et */
int				is_double_quoted_literal(char *str);
char			*extract_single_quote_content(char *str);				/* Tek tırnaklı içerik çıkart */

int				is_builtin(char *command);								/* Built-in komut mu kontrol et */
int				execute_builtin(t_command *cmd, t_global *global, int *originals, int flag);		/* Built-in komut çalıştır */
// int				builtin_pwd(void);										/* pwd built-in */
int				builtin_echo(char **args);								/* echo built-in */
int				builtin_env(t_env *env_list);							/* env built-in */
int				builtin_exit(char **args);			/* exit built-in */
int				builtin_cd(char **args, t_global *global);				/* cd built-in */
int				builtin_export(char **args, t_global *global);			/* export built-in */
int				builtin_unset(char **args, t_global *global);			/* unset built-in */

void			update_pwd_env(t_global *global);						/* PWD çevre değişkenini güncelle */
void			print_export_env(t_env *env_list);						/* Export formatında çevre değişkenlerini yazdır */
void			set_env_var(t_global *global, char *key, char *value);	/* Çevre değişkeni ayarla */
void			unset_env_var(t_global *global, char *key);			/* Çevre değişkeni sil */
char			*resolve_logical_path(char *current_pwd, char *path);	/* Logical path çözümle */
char			*find_existing_parent(char *path);					/* Var olan en yakın parent bul */

int				execute_commands(t_command *commands, t_global *global);	/* Ana execution fonksiyonu */
int				execute_external_command(t_command *cmd, t_global *global);	/* External komut çalıştır */
int				execute_pipeline(t_command *commands, t_global *global);	/* Pipeline çalıştır */
int				execute_pipeline_command(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd);
pid_t			execute_pipeline_command_async(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd);

char			*find_command_path(char *command, t_env *env_list);		/* Komut path'ini bul */
char			*get_env_value(t_env *env_list, char *key);				/* Çevre değişkeni değerini al */
char			**env_list_to_array(t_env *env_list);					/* Çevre listesini array'e çevir */
int				count_env_nodes(t_env *env_list);						/* Çevre düğümlerini say */

int				setup_redirections(t_command *cmd);					/* Yönlendirmeleri ayarla */
void			setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd);	/* Pipeline fd ayarla */
int				handle_heredoc(t_redirect *redirect);						/* Heredoc işle */

void			setup_signals(void);									/* Sinyalleri ayarla */
void			sigint_handler(int sig);								/* SIGINT (Ctrl+C) handler */
void			setup_child_signals(void);								/* Child process sinyalleri */
void			handle_eof(void);										/* EOF (Ctrl+D) işleme */
void			print_welcome_advanced(void);

char			*ft_strjoin_char(char const *s1, char const s2);
void			skip_whitespace_advanced(t_lexer_new *lexer);
void			execute_redirect_command(t_command *cmd, t_global *global, int *origin);
int				preprocess_heredocs(t_command *commands, t_global *global);
int				preprocess_handle_heredoc(t_redirect *redirect);
int				wait_for_redirect_process(pid_t pid);

void			handle_single_assignment(char *arg, t_global *global);
void			handle_split_assignment(char **args, int i, t_global *global);
int				is_valid_key_char(char *arg, char *equal_pos);
void			print_error(void);

#endif
