#ifndef __TOKENIZE__
#define __TOKENIZE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"


#define MAX_TOKENS      400
#define KEYWORD_INT     "int"
#define KEYWORD_INT_LEN 3
#define KEYWORD_IF      "if"
#define KEYWORD_IF_LEN  2

typedef enum  {
    TokenType_keyword_if,
    TokenType_keyword_int,
    TokenType_number,
    TokenType_operator_add,
    TokenType_operator_subtract,
    TokenType_operator_equal,
    TokenType_operator_assign,
    TokenType_identifier,
    TokenType_semicolon,
    TokenType_lbrack,
    TokenType_rbrack,
    TokenType_lbrace,
    TokenType_rbrace,
    TokenTypeUndefined,

}TokenType;

typedef struct {
    TokenType token_type;
    size_t str_len;
    const char* str;

} Token;

typedef struct {
    Token *token;
    size_t no_of_tokens;
} Tokens;

void print_token(const Token *token);
void print_tokens(Tokens tokens);
Tokens tokenize(File file);

#endif