/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 17:13:26 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/11 17:16:30 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_ENUM_H
# define TOKEN_ENUM_H

typedef enum	s_token_type
{
    TOKEN_WORD,          // Normal kelimeler: ls, echo, /bin/cat
    TOKEN_PIPE,          // |
    TOKEN_REDIRECT_IN,   // <
    TOKEN_REDIRECT_OUT,  // >
    TOKEN_APPEND,        // >>
    TOKEN_HEREDOC,       // <<
    TOKEN_SINGLE_QUOTE,  // 'string'
    TOKEN_DOUBLE_QUOTE,  // "string"
    TOKEN_ENV_VAR,       // $USER, $HOME
    TOKEN_WHITESPACE,    // boşluk, tab (genelde skip ederiz)
    TOKEN_EOF            // input sonu
}	t_token_type;

#endif