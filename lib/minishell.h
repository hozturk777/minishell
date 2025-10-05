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
# define PROMPT	"minishell$ "
# define BLACK	"\033[1;90m"
# define GREEN	"\033[1;92m"
# define RESET	"\033[0m"
# define MAX_HEREDOC_FDS 32

# include "../include/libft/libft.h"
# include <stdlib.h>
# include <fcntl.h>

typedef struct s_global	t_global;
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
	t_token_types	type;
	char			*value;
	int				len;
	int				quote_type;
	int				expanded;
}	t_token_new;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_redirect
{
	t_token_types		type;
	char				*filename;
	int					fd;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_command
{
	char				**args;
	t_list				*redirections;
	int					pipe_fd[2];
	pid_t				pid;
	struct s_command	*next;
}	t_command;

typedef struct s_lexer_new
{
	char		*input;
	int			pos;
	int			len;
	int			t_cmd_flag;
	char		current_char;
	t_global	*global;
}	t_lexer_new;

typedef struct s_global
{
	t_list		*tokens;
	t_command	*commands;
	t_env		*env_list;
	int			exit_status;
	int			heredoc_count;
	char		*input_line;
	int			in_child;
	int			should_exit;
	int			echo_flag;
	int			heredoc_fds[MAX_HEREDOC_FDS];
	int			heredoc_fd_count;
}	t_global;

t_global		*get_global(void);

t_list			*tokenize_advanced(char *input, t_global *global);
t_token_new		*create_token_advanced(t_token_types type, char *value);
t_lexer_new		*init_lexer_advanced(char *input, t_global *global);
void			advance_lexer(t_lexer_new *lexer);

t_env			*create_env_node(char *key, char *value);
void			add_env_node(t_env **env_list, t_env *new_node);
t_env			*find_env_node(t_env *env_list, char *key);
t_env			*init_env_from_envp(char **envp);

t_token_new		*handle_pipe_advanced(t_lexer_new *lexer);
t_token_new		*handle_redirect_advanced(t_lexer_new *lexer);
t_token_new		*handle_word_advanced(t_lexer_new *lexer);
t_token_new		*handle_quotes_advanced(t_lexer_new *lexer);
t_token_new		*handle_whitespaces_advanced(t_lexer_new *lexer);

t_command		*create_command(void);
t_command		*parse_tokens_to_commands(t_list *tokens, t_global *global);

t_global		*init_global(char **envp, t_global *global);
void			register_heredoc_fd(int fd);
void			close_all_heredoc_fds(void);
void			cleanup_and_exit(void);

t_command		*parse_single_command(t_list **token_node, t_global *global);
int				is_command_start(t_list *token_node);
int				is_pipe_token(t_list *token_node);
int				is_redirect_token(t_list *token_node);
int				is_word_token(t_list *token_node);
char			**convert_list_to_array(t_list *args_list);
void			append_command_to_chain(t_command *head, t_command *new_cmd);
void			add_redirect_to_command(t_command *cmd, t_redirect *redirect);

char			*handle_dollar_expansion(char *input, int *i, t_global *global);
char			*handle_regular_char(char *input, int *i);

char			*expand_with_quotes(char *input, t_global *global);
char			*expand_with_heredoc(char *input, t_global *global);
char			*remove_outer_quotes(char *input);
int				needs_expansion(char *str);
void			expand_command_args(t_command *cmd, t_global *global);
int				is_single_quoted_literal(char *str);
int				is_double_quoted_literal(char *str);
char			*extract_single_quote_content(char *str);
int				is_builtin(char *command);
int				execute_builtin(t_command *cmd, t_global *global,
					int *originals, int flag);
int				builtin_echo(char **args);
int				builtin_env(t_env *env_list);
int				builtin_exit(char **args);
int				builtin_cd(char **args, t_global *global);
int				builtin_export(char **args, t_global *global);
int				builtin_unset(char **args, t_global *global);
void			update_pwd_env(t_global *global);
void			print_export_env(t_env *env_list);
void			set_env_var(t_global *global, char *key, char *value);
void			unset_env_var(t_global *global, char *key);
char			*resolve_logical_path(char *current_pwd, char *path);
char			*find_existing_parent(char *path);
int				execute_commands(t_command *commands, t_global *global);
int				execute_external_command(t_command *cmd, t_global *global);
int				execute_pipeline(t_command *commands, t_global *global);
int				execute_pipeline_command(t_command *cmd, t_global *global,
					int prev_fd, int *pipe_fd);
pid_t			execute_pipeline_command_async(t_command *cmd, t_global *global,
					int prev_fd, int *pipe_fd);
char			*find_command_path(char *command, t_env *env_list);
char			*get_env_value(t_env *env_list, char *key);
char			**env_list_to_array(t_env *env_list);
int				count_env_nodes(t_env *env_list);
int				setup_redirections(t_command *cmd);
void			setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd);
int				handle_heredoc(t_redirect *redirect);
void			setup_signals(void);
void			sigint_handler(int sig);
void			setup_child_signals(void);
void			handle_eof(void);
void			print_welcome_advanced(void);
char			*ft_strjoin_char(char const *s1, char const s2);
void			skip_whitespace_advanced(t_lexer_new *lexer);
void			execute_redirect_command(t_command *cmd, t_global *global,
					int *origin);
int				preprocess_heredocs(t_command *commands);
int				preprocess_handle_heredoc(t_redirect *redirect);
void			preprocess_heredoc_lines(t_redirect *redirect, int fd,
					t_global *global);
int				wait_for_redirect_process(pid_t pid);
void			handle_single_assignment(char *arg, t_global *global);
void			handle_split_assignment(char **args, int i, t_global *global);
int				is_valid_key_char(char *arg, char *equal_pos);
void			print_error(void);
int				preprocess_heredocs_utils(t_command *current);
void			setup_redirections_check(t_command *cmd, int *originals);
void			cleanup_and_close(int *originals);
void			is_builtin_check(t_command *cmd, t_global *global,
					int *originals, int *exit_num);
void			close_unused_heredoc_fds(t_command *cmd);
int				redirect_check(t_command *command);
int				heredoc_check(t_command	*command);
void			check_path(t_command *cmd, t_list *node, int *originals);
int				heredoc_check_utils(t_redirect *redirect);
void			child_run_execve(t_command *cmd, int *originals);
void			pipeline_close_helper(int	*prev_fd, t_command **current,
					int *pipe_fd);
int				check_token_at_index(t_global *global, int index);
char			*merge_quoted_args(char **args, int s_index,
					int *e_index, t_global *global);
int				handle_quoted_export(char **args, int i,
					t_global *global, int *end_index);
int				is_equal_export(char *equal_pos, char *merged_arg,
					t_global *global);
void			expand_double_quotes(t_command *cmd, t_global *global, int *i);
void			expand_single_quotes(t_command *cmd, int *i);
void			need_expansion_utils(t_command *cmd, t_global *global, int *i);
int				check_syntax_redirect(t_list **token_temp,
					t_token_new **token_test);
int				check_syntax_heredoc(t_list **token_temp,
					t_token_new **token_test);
void			setup_child_signals_sigdfl(void);
int				wait_for_external_process(pid_t pid);
void			print_signal_message(int signal_num);

#endif
