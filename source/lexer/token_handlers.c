/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 12:00:00 by huozturk          #+#    #+#             */
/*   Updated: 2025/09/29 09:36:29 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../lib/minishell.h"

t_token_new	*handle_pipe_advanced(t_lexer_new *lexer)
{
	advance_lexer(lexer);
	return (create_token_advanced(T_PIPE, "|"));
}

// t_token_new	*handle_redirect_advanced(t_lexer_new *lexer)
// {
// 	//int		start_pos;

// 	//start_pos = lexer->pos;
// 	if (lexer->current_char == '<')
// 	{
// 		advance_lexer(lexer);
// 		while (lexer->current_char == ' ' || lexer->current_char == '\t')
// 			advance_lexer(lexer);
// 		if (lexer->current_char == '<')
// 		{
// 			advance_lexer(lexer);
// 			while (lexer->current_char == ' ' || lexer->current_char == '\t')
// 			    advance_lexer(lexer);
// 			if (!ft_isprint(lexer->current_char) || lexer->current_char == '|') // Burada returnde value dönüp çıktıyı değiştiricez
// 				return (NULL);
// 			return (create_token_advanced(T_HEREDOC, "<<"));
// 		}
// 		else if (!ft_isprint(lexer->current_char) || lexer->current_char == '|') // Burada returnde value dönüp çıktıyı değiştiricez
// 			return (NULL);
// 		return (create_token_advanced(T_REDIRECT_IN, "<"));
// 	}
// 	else if (lexer->current_char == '>')
// 	{
// 		advance_lexer(lexer);
// 		while (lexer->current_char == ' ' || lexer->current_char == '\t')
// 			    advance_lexer(lexer);
// 		if (lexer->current_char == '>')
// 		{
// 			advance_lexer(lexer);
// 			while (lexer->current_char == ' ' || lexer->current_char == '\t')
// 			    advance_lexer(lexer);
// 			if (!ft_isprint(lexer->current_char) || lexer->current_char == '|') // Burada returnde value dönüp çıktıyı değiştiricez
// 				return (NULL);
// 			return (create_token_advanced(T_APPEND, ">>"));
// 		}
// 		else if (!ft_isprint(lexer->current_char) || lexer->current_char == '|') // Burada returnde value dönüp çıktıyı değiştiricez
// 			return (NULL);
// 		return (create_token_advanced(T_REDIRECT_OUT, ">"));
// 	}
// 	return (NULL);
// }
// static t_token_new	*handle_input_redirect(t_lexer_new *lexer)
// {
// 	advance_lexer(lexer);
// 	skip_whitespace_advanced(lexer);
// 	if (lexer->current_char == '<')
// 	{
// 		advance_lexer(lexer);
// 		skip_whitespace_advanced(lexer);
// 		/* Burada returnde value dönüp çıktıyı değiştiricez */
// 		if (!ft_isprint(lexer->current_char) || lexer->current_char == '|')
// 			return (NULL);
// 		return (create_token_advanced(T_HEREDOC, "<<"));
// 	}
// 	/* Burada returnde value dönüp çıktıyı değiştiricez */
// 	else if (!ft_isprint(lexer->current_char) || lexer->current_char == '|')
// 		return (NULL);
// 	return (create_token_advanced(T_REDIRECT_IN, "<"));
// }

// static t_token_new	*handle_output_redirect(t_lexer_new *lexer)
// {
// 	advance_lexer(lexer);
// 	skip_whitespace_advanced(lexer);
// 	if (lexer->current_char == '>')
// 	{
// 		advance_lexer(lexer);
// 		skip_whitespace_advanced(lexer);
// 		/* Burada returnde value dönüp çıktıyı değiştiricez */
// 		if (!ft_isprint(lexer->current_char) || lexer->current_char == '|')
// 			return (NULL);
// 		return (create_token_advanced(T_APPEND, ">>"));
// 	}
// 	/* Burada returnde value dönüp çıktıyı değiştiricez */
// 	else if (!ft_isprint(lexer->current_char) || lexer->current_char == '|')
// 		return (NULL);
// 	return (create_token_advanced(T_REDIRECT_OUT, ">"));
// }

// t_token_new	*handle_redirect_advanced(t_lexer_new *lexer)
// {
//     /* int		start_pos; */
//     /* start_pos = lexer->pos; */
//     if (lexer->current_char == '<')
//         return (handle_input_redirect(lexer));
//     else if (lexer->current_char == '>')
//         return (handle_output_redirect(lexer));
//     return (NULL);
// }

static t_token_new	*extract_regular_word(t_lexer_new *lexer, int start, t_token_types types)
{
	int		len;
	char	*word;

	while (lexer->current_char != '\0' && lexer->current_char != ' '
		&& lexer->current_char != '\t' && lexer->current_char != '\n'
		&& lexer->current_char != '|' && lexer->current_char != '<'
		&& lexer->current_char != '>' && lexer->current_char != '\''
		&& lexer->current_char != '"')
		advance_lexer(lexer);
	len = lexer->pos - start;
	if (len > 0)
	{
		/* code */
		word = ft_substr(lexer->input, start, len);
	}
	else
		return (NULL);
	if (!word)
		return (NULL);
	/* if (types == T_CMD) */
	/*     advance_lexer(lexer); */
	return (create_token_advanced(types, word));
}

static t_token_new	*process_word_content(t_lexer_new *lexer, int start, t_token_types types)
{
    int		len;
    char	*word;

    /* Special case for $"..." pattern - treat as single word */
    if (lexer->current_char == '$' && lexer->input[lexer->pos + 1] == '"')
    {
        advance_lexer(lexer);
        advance_lexer(lexer);
        /* Read until closing quote */
        while (lexer->current_char != '\0' && lexer->current_char != '"')
            advance_lexer(lexer);
        if (lexer->current_char == '"')
            advance_lexer(lexer);
        len = lexer->pos - start;
        word = ft_substr(lexer->input, start + 2, len - 3);
        if (!word)
            return (NULL);
        return (create_token_advanced(types, word));
    }
    return (extract_regular_word(lexer, start, types));
}

t_token_new	*handle_word_advanced(t_lexer_new *lexer, int *first_word_check)
{
    int				start;
    int				len;
    char			*word;
    t_token_types	types;

    start = lexer->pos;
    len = 0;
    word = NULL;
    types = T_WORD;
    if (!lexer || !lexer->input || lexer->pos >= lexer->len)
        return (NULL);
    /* Pipedan sonra ki inputu da T_CMD işaretliyecez & -n -e -E'de başta boşluk bırakıyordu T_CMD çevirdik token tipini */
    if (lexer->pos == 0 || lexer->t_cmd_flag == 1)
    {
        types = T_CMD;
        *first_word_check = 0;
        lexer->t_cmd_flag = 0;
    }
    else
    {
        types = T_WORD;
        *first_word_check = 1;
    }
    return (process_word_content(lexer, start, types));
}

// t_token_new	*handle_word_advanced(t_lexer_new *lexer, int *first_word_check)
// {
// 	int		start;
// 	int		len;
// 	char	*word;
// 	t_token_types types;


// 	start = lexer->pos;
// 	len = 0;
// 	word = NULL;
// 	types = T_WORD;
// 	if (!lexer || !lexer->input || lexer->pos >= lexer->len)
// 		return (NULL);

// 	if (lexer->pos == 0 || lexer->t_cmd_flag == 1 || (lexer->current_char == '-' && (lexer->input[lexer->pos + 1] == 'n' || lexer->input[lexer->pos + 1] == 'e' || lexer->input[lexer->pos + 1] == 'E'))) // Pipedan sonra ki inputu da T_CMD işaretliyecez & -n -e -E'de başta boşluk bırakıyordu T_CMD çevirdik token tipini
// 	{
// 		types = T_CMD;
// 		*first_word_check = 0;
// 		lexer->t_cmd_flag = 0;
// 	}
// 	else
// 	{
// 		types = T_WORD;
// 		*first_word_check = 1;
// 	}
	
// 	// Special case for $"..." pattern - treat as single word
// 	if (lexer->current_char == '$' && lexer->input[lexer->pos + 1] == '"')
// 	{
// 		advance_lexer(lexer); // Skip $
// 		advance_lexer(lexer); // Skip "
// 		// Read until closing quote
// 		while (lexer->current_char != '\0' && lexer->current_char != '"')
// 			advance_lexer(lexer);
// 		if (lexer->current_char == '"')
// 			advance_lexer(lexer); // Skip closing "
// 		len = lexer->pos - start;
// 		word = ft_substr(lexer->input, start + 2, len - 3); // Skip $" and "
// 		if (!word)
// 			return (NULL);
// 		return (create_token_advanced(types, word));
// 	}
	
// 	while (lexer->current_char != '\0' && lexer->current_char != ' '
// 		&& lexer->current_char != '\t' && lexer->current_char != '\n'
// 		&& lexer->current_char != '|' && lexer->current_char != '<'
// 		&& lexer->current_char != '>' && lexer->current_char != '\''
// 		&& lexer->current_char != '"')
// 		advance_lexer(lexer);
// 	len = lexer->pos - start;
// 	if (len > 0)
// 	{
// 		/* code */
// 		word = ft_substr(lexer->input, start, len);
// 	}
// 	else
// 		return (NULL);
	
// 	if (!word)
// 		return (NULL);
// 	// if (types == T_CMD)
// 	//     advance_lexer(lexer);
// 	return (create_token_advanced(types, word));
// }


// t_token_new	*handle_quotes_advanced(t_lexer_new *lexer)
// {
// 	char	quote_char;
// 	int		start;
// 	int		len;
// 	char	*quoted_str;

// 	quote_char = lexer->current_char; 
// 	advance_lexer(lexer);
// 	start = lexer->pos;
// 	while (lexer->current_char != '\0' && lexer->current_char != quote_char)
// 		advance_lexer(lexer);
// 	if (lexer->current_char == quote_char)
// 	{
// 		len = lexer->pos - start;
// 		// printf("lexer: $%c$ - LEM: $%d$\n", lexer->current_char, len);
		
// 		quoted_str = ft_substr(lexer->input, start, len);
// 		advance_lexer(lexer);
// 		if (len == 0)
// 			return(create_token_advanced(T_SINGLE_QUOTE, ft_strdup("")));
// 		if (quote_char == '\'')
// 			return (create_token_advanced(T_SINGLE_QUOTE, quoted_str));
// 		else
// 			return (create_token_advanced(T_DOUBLE_QUOTE, quoted_str));
// 	}
// 	return (NULL);
// }

// t_token_new	*handle_whitespaces_advanced(t_lexer_new *lexer)
// {
// 	int		start;
// 	int		len;
// 	char	*whitepaces;

// 	start = lexer->pos;
// 	// if (lexer->first_word_check == 0)
// 	// {
// 	// 	while (lexer->current_char == ' ' || lexer->current_char == '\t')
//     //         advance_lexer(lexer);
// 	// }
	
// 	while (lexer->current_char != '\0' &&(lexer->current_char == ' ' || lexer->current_char == '\t'))
// 	{
// 		advance_lexer(lexer);
// 	}
// 	len = lexer->pos - start;
// 	// whitepaces = ft_substr(lexer->input, start, len);
// 	whitepaces = " ";
// 	return(create_token_advanced(T_WHITESPACE, whitepaces));
// }
