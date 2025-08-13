/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 21:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/13 21:00:00 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include "../include/libft/libft.h"

char	*ft_strndup(const char *s, size_t n)
{
	char	*result;
	size_t	i;

	if (!s)
		return (NULL);
	result = malloc(n + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (i < n && s[i])
	{
		result[i] = s[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}


int	main(int ac, char *av[], char **env)
{
	char	*input;
	t_token	*tokens;

	(void)ac;
	(void)av;
	(void)env;
	print_welcome();
	printf("\n=== LEXER TEST MODE ===\n");
	printf("Test komutları:\n");
	printf("- echo hello\n");
	printf("- ls | grep main\n");
	printf("- echo \"hello world\"\n");
	printf("- cat > file.txt\n");
	printf("- exit (çıkmak için)\n\n");
	while (1)
	{
		input = readline(GREEN PROMPT RESET);
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (*input)
		{
			add_history(input);
			tokens = tokenize(input);
			print_tokens(tokens);
			free_tokens(tokens);
		}
		if (ft_strncmp(input, "exit", 4) == 0 && ft_strlen(input) == 4)
		{
			free(input);
			break ;
		}
		free(input);
	}
	return (0);
}
t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	if (value)
		token->len = ft_strlen(value);
	else
		token->len = 0;
	token->next = NULL;
	return (token);
}

void	advance(t_lexer *lexer)
{
	lexer->pos++;
	if (lexer->pos >= lexer->len)
		lexer->current_char = '\0';
	else
		lexer->current_char = lexer->input[lexer->pos];
}

void	skip_whitespace(t_lexer *lexer)
{
	while (lexer->current_char == ' ' || lexer->current_char == '\t'
		|| lexer->current_char == '\n')
		advance(lexer);
}

t_token	*handle_pipe(t_lexer *lexer)
{
	t_token	*token;

	token = create_token(TOKEN_PIPE, "|");
	advance(lexer);
	return (token);
}

t_token	*handle_redirect_out(t_lexer *lexer)
{
	t_token	*token;

	advance(lexer);
	if (lexer->current_char == '>')
	{
		token = create_token(TOKEN_APPEND, ">>");
		advance(lexer);
	}
	else
		token = create_token(TOKEN_REDIRECT_OUT, ">");
	return (token);
}

t_token	*handle_redirect_in(t_lexer *lexer)
{
	t_token	*token;

	advance(lexer);
	if (lexer->current_char == '<')
	{
		token = create_token(TOKEN_HEREDOC, "<<");
		advance(lexer);
	}
	else
		token = create_token(TOKEN_REDIRECT_IN, "<");
	return (token);
}

t_token	*handle_double_quote(t_lexer *lexer)
{
	char	*value;
	int		start_pos;
	int		len;
	t_token	*token;

	advance(lexer);
	start_pos = lexer->pos;
	while (lexer->current_char != '"' && lexer->current_char != '\0')
		advance(lexer);
	len = lexer->pos - start_pos;
	value = ft_strndup(lexer->input + start_pos, len);
	if (lexer->current_char == '"')
		advance(lexer);
	token = create_token(TOKEN_DOUBLE_QUOTE, value);
	free(value);
	return (token);
}

t_token	*handle_single_quote(t_lexer *lexer)
{
	char	*value;
	int		start_pos;
	int		len;

	advance(lexer);
	start_pos = lexer->pos;
	while (lexer->current_char != '\'' && lexer->current_char != '\0')
		advance(lexer);
	len = lexer->pos - start_pos;
	value = ft_strndup(lexer->input + start_pos, len);
	if (lexer->current_char == '\'')
		advance(lexer);
	return (create_token(TOKEN_SINGLE_QUOTE, value));
}

t_token	*handle_word(t_lexer *lexer)
{
	char	*value;
	int		start_pos;
	int		len;
	t_token	*token;

	start_pos = lexer->pos;
	while (lexer->current_char != '\0' && lexer->current_char != ' '
		&& lexer->current_char != '\t' && lexer->current_char != '|'
		&& lexer->current_char != '>' && lexer->current_char != '<'
		&& lexer->current_char != '"' && lexer->current_char != '\'')
		advance(lexer);
	len = lexer->pos - start_pos;
	value = ft_strndup(lexer->input + start_pos, len);
	token = create_token(TOKEN_WORD, value);
	free(value);
	return (token);
}

void	add_token_to_list(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!new_token)
		return ;
	if (*tokens == NULL)
		*tokens = new_token;
	else
	{
		current = *tokens;
		while (current->next)
			current = current->next;
		current->next = new_token;
	}
}

void	free_lexer(t_lexer *lexer)
{
	if (lexer)
		free(lexer);
}

t_token	*tokenize(char *input)
{
	t_lexer	*lexer;
	t_token	*tokens;
	t_token	*current_token;

	lexer = init_lexer(input);
	if (!lexer)
		return (NULL);
	tokens = NULL;
	while (lexer->current_char != '\0')
	{
		skip_whitespace(lexer);
		if (lexer->current_char == '\0')
			break ;
		if (lexer->current_char == '|')
			current_token = handle_pipe(lexer);
		else if (lexer->current_char == '>')
			current_token = handle_redirect_out(lexer);
		else if (lexer->current_char == '<')
			current_token = handle_redirect_in(lexer);
		else if (lexer->current_char == '"')
			current_token = handle_double_quote(lexer);
		else if (lexer->current_char == '\'')
			current_token = handle_single_quote(lexer);
		else
			current_token = handle_word(lexer);
		if (current_token)
			add_token_to_list(&tokens, current_token);
	}
	free_lexer(lexer);
	return (tokens);
}

t_lexer	*init_lexer(char *input)
{
	t_lexer	*lexer;

	if (!input)
		return (NULL);
	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->pos = 0;
	lexer->len = ft_strlen(input);
	if (lexer->len > 0)
		lexer->current_char = input[0];
	else
		lexer->current_char = '\0';
	return (lexer);
}

// Debug fonksiyonu: Token'ları yazdır
static void	print_token_type(t_token_type type)
{
	if (type == TOKEN_WORD)
		printf("WORD");
	else if (type == TOKEN_PIPE)
		printf("PIPE");
	else if (type == TOKEN_REDIRECT_IN)
		printf("REDIRECT_IN");
	else if (type == TOKEN_REDIRECT_OUT)
		printf("REDIRECT_OUT");
	else if (type == TOKEN_APPEND)
		printf("APPEND");
	else if (type == TOKEN_HEREDOC)
		printf("HEREDOC");
	else if (type == TOKEN_SINGLE_QUOTE)
		printf("SINGLE_QUOTE");
	else if (type == TOKEN_DOUBLE_QUOTE)
		printf("DOUBLE_QUOTE");
	else if (type == TOKEN_ENV_VAR)
		printf("ENV_VAR");
	else if (type == TOKEN_WHITESPACE)
		printf("WHITESPACE");
	else if (type == TOKEN_EOF)
		printf("EOF");
	else
		printf("UNKNOWN");
}

void	print_tokens(t_token *tokens)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	printf("\n=== TOKEN LIST ===\n");
	if (!tokens)
	{
		printf("No tokens found.\n");
		printf("==================\n\n");
		return ;
	}
	while (current)
	{
		printf("Token %d: [%s] Type: %d (", i, current->value, current->type);
		print_token_type(current->type);
		printf(")\n");
		current = current->next;
		i++;
	}
	printf("==================\n\n");
}

void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}
