/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				len;
	struct s_token	*next;
}	t_token;

typedef struct s_lexer
{
	char	*input;
	int		pos;
	int		len;
	char	current_char;
}	t_lexer;

t_token		*tokenize(char *input);
t_token		*create_token(t_token_type type, char *value);
void		free_tokens(t_token *tokens);
t_lexer		*init_lexer(char *input);
void		free_lexer(t_lexer *lexer);
void		advance(t_lexer *lexer);
void		skip_whitespace(t_lexer *lexer);
void		print_tokens(t_token *tokens);
void		print_welcome(void);
char		*ft_strndup(const char *s, size_t n);
t_token		*handle_pipe(t_lexer *lexer);
t_token		*handle_redirect_out(t_lexer *lexer);
t_token		*handle_redirect_in(t_lexer *lexer);
t_token		*handle_double_quote(t_lexer *lexer);
t_token		*handle_single_quote(t_lexer *lexer);
t_token		*handle_word(t_lexer *lexer);
void		add_token_to_list(t_token **tokens, t_token *new_token);

#endif
