/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 18:03:28 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/12 14:08:41 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/minishell.h"
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include "../lib/minishell.h"
#include "../include/libft/libft.h"


t_token	*tokenize(char *input);
t_lexer	*init_lexer(char *input);

// strndup fonksiyonu (bazı sistemlerde olmayabilir)
char *ft_strndup(const char *s, size_t n)
{
    char *result;
    size_t i;

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


int main(int ac, char *av[], char **env)
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
			
			// Test: Input'u tokenize et
			tokens = tokenize(input);
			
			// Debug: Token'ları yazdır
			print_tokens(tokens);
			
			// Memory temizle
			free_tokens(tokens);
		}
		
		// Exit komutu kontrolü
		if (strcmp(input, "exit") == 0)
		{
			free(input);
			break;
		}
		
		free(input);
	}
	return (0);	
}
t_token *create_token(t_token_type type, char *value)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    token->type = type;
    token->value = strdup(value);
    if (value)
        token->len = strlen(value);
    else
        token->len = 0;
    token->next = NULL;
    return (token);
}

void advance(t_lexer *lexer)
{
    lexer->pos++;
    if (lexer->pos >= lexer->len)
        lexer->current_char = '\0';
    else
        lexer->current_char = lexer->input[lexer->pos];
}

void skip_whitespace(t_lexer *lexer)
{
    while (lexer->current_char == ' ' || 
           lexer->current_char == '\t' || 
           lexer->current_char == '\n')
    {
        advance(lexer);
    }
}

t_token *handle_pipe(t_lexer *lexer)
{
    t_token *token;
    
    token = create_token(TOKEN_PIPE, "|");
    advance(lexer);  // '|' karakterini geç
    return (token);
}

t_token *handle_redirect_out(t_lexer *lexer)
{
    t_token *token;
    
    advance(lexer);  // İlk '>' karakterini geç
    
    // ">>" kontrolü (append)
    if (lexer->current_char == '>')
    {
        token = create_token(TOKEN_APPEND, ">>");
        advance(lexer);  // İkinci '>' karakterini de geç
    }
    else
    {
        token = create_token(TOKEN_REDIRECT_OUT, ">");
    }
    
    return (token);
}

t_token *handle_redirect_in(t_lexer *lexer)
{
    t_token *token;
    
    advance(lexer);  // İlk '<' karakterini geç
    
    // "<<" kontrolü (heredoc)
    if (lexer->current_char == '<')
    {
        token = create_token(TOKEN_HEREDOC, "<<");
        advance(lexer);  // İkinci '<' karakterini de geç
    }
    else
    {
        token = create_token(TOKEN_REDIRECT_IN, "<");
    }
    
    return (token);
}

t_token *handle_double_quote(t_lexer *lexer)
{
    char *value;
    int start_pos;
    int len;
    t_token *token;
    
    advance(lexer);  // İlk '"' karakterini geç
    start_pos = lexer->pos;
    
    // Kapanış tırnağını bul
    while (lexer->current_char != '"' && lexer->current_char != '\0')
        advance(lexer);
    
    len = lexer->pos - start_pos;
    value = ft_strndup(lexer->input + start_pos, len);
    
    if (lexer->current_char == '"')
        advance(lexer);  // Kapanış tırnağını geç
    
    token = create_token(TOKEN_DOUBLE_QUOTE, value);
    free(value);  // Memory leak'i önle
    return (token);
}

t_token *handle_single_quote(t_lexer *lexer)
{
    char *value;
    int start_pos;
    int len;
    
    advance(lexer);  // İlk '\'' karakterini geç
    start_pos = lexer->pos;
    
    // Kapanış tırnağını bul
    while (lexer->current_char != '\'' && lexer->current_char != '\0')
        advance(lexer);
    
    len = lexer->pos - start_pos;
    value = ft_strndup(lexer->input + start_pos, len);
    
    if (lexer->current_char == '\'')
        advance(lexer);  // Kapanış tırnağını geç
    
    return (create_token(TOKEN_SINGLE_QUOTE, value));
}

t_token *handle_word(t_lexer *lexer)
{
    char *value;
    int start_pos;
    int len;
    t_token *token;
    
    start_pos = lexer->pos;
    
    // Kelime sonuna kadar ilerle
    while (lexer->current_char != '\0' && 
           lexer->current_char != ' ' && 
           lexer->current_char != '\t' && 
           lexer->current_char != '|' && 
           lexer->current_char != '>' && 
           lexer->current_char != '<' && 
           lexer->current_char != '"' && 
           lexer->current_char != '\'')
    {
        advance(lexer);
    }
    
    len = lexer->pos - start_pos;
    value = ft_strndup(lexer->input + start_pos, len);
    
    token = create_token(TOKEN_WORD, value);
    free(value);  // Memory leak'i önle
    return (token);
}
void add_token_to_list(t_token **tokens, t_token *new_token)
{
    t_token *current;
    
    if (!new_token)
        return;
    
    if (*tokens == NULL)
    {
        *tokens = new_token;  // İlk token
    }
    else
    {
        // Son token'ı bul
        current = *tokens;
        while (current->next)
            current = current->next;
        
        // Yeni token'ı ekle
        current->next = new_token;
    }
}

void free_lexer(t_lexer *lexer)
{
    if (lexer)
    {
        // input string'i free etmiyoruz çünkü dışarıdan geldi
        free(lexer);
    }
}

t_token	*tokenize(char *input)
{
	t_lexer	*lexer = init_lexer(input);
	t_token	*tokens = NULL;
	t_token	*current_token = NULL;

	if (!lexer)
		return (NULL);

	while (lexer->current_char != '\0')
    {
        skip_whitespace(lexer);
        
        // Eğer whitespace'den sonra string bittiyse çık
        if (lexer->current_char == '\0')
            break;
        
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
            
        // Token'ı listeye ekle
        if (current_token)
            add_token_to_list(&tokens, current_token);
    }
    
    free_lexer(lexer);
    return (tokens);
}

t_lexer *init_lexer(char *input)
{
    t_lexer *lexer;

    if (!input)
        return (NULL);
        
    lexer = malloc(sizeof(t_lexer));
    if (!lexer)
        return (NULL);
        
    lexer->input = input;
    lexer->pos = 0;                    
    lexer->len = strlen(input);        
    
    if (lexer->len > 0)
        lexer->current_char = input[0]; 
    else
        lexer->current_char = '\0';
        
    return (lexer);
}

// Debug fonksiyonu: Token'ları yazdır
void print_tokens(t_token *tokens)
{
    t_token *current = tokens;
    int i = 0;
    
    printf("\n=== TOKEN LIST ===\n");
    if (!tokens)
    {
        printf("No tokens found.\n");
        printf("==================\n\n");
        return;
    }
    
    while (current)
    {
        printf("Token %d: [%s] Type: %d (", i, current->value, current->type);
        
        // Token türünü yazdır
        switch(current->type)
        {
            case TOKEN_WORD: printf("WORD"); break;
            case TOKEN_PIPE: printf("PIPE"); break;
            case TOKEN_REDIRECT_IN: printf("REDIRECT_IN"); break;
            case TOKEN_REDIRECT_OUT: printf("REDIRECT_OUT"); break;
            case TOKEN_APPEND: printf("APPEND"); break;
            case TOKEN_HEREDOC: printf("HEREDOC"); break;
            case TOKEN_SINGLE_QUOTE: printf("SINGLE_QUOTE"); break;
            case TOKEN_DOUBLE_QUOTE: printf("DOUBLE_QUOTE"); break;
            case TOKEN_ENV_VAR: printf("ENV_VAR"); break;
            case TOKEN_WHITESPACE: printf("WHITESPACE"); break;
            case TOKEN_EOF: printf("EOF"); break;
            default: printf("UNKNOWN"); break;
        }
        
        printf(")\n");
        current = current->next;
        i++;
    }
    printf("==================\n\n");
}

// Memory temizleme fonksiyonu
void free_tokens(t_token *tokens)
{
    t_token *current = tokens;
    t_token *next;
    
    while (current)
    {
        next = current->next;
        if (current->value)
            free(current->value);
        free(current);
        current = next;
    }
}
