#include "tokenize.h"
#include <stdio.h>

void print_token(const Token *token) {
    if (!token){
        puts("NULL");
        return;
    }
    
    printf (
        "token type: %d, token len: %zu, token: %.*s\n",
        token->token_type, 
        token->str_len, 
        token->str_len, 
        token->str
    );

}

void print_tokens(const Tokens tokens) {
    puts("\nTOKENS:");
    for (int i = 0 ; i < tokens.no_of_tokens ; i++) {
        printf ("token type: %d, token len: %zu, token: %.*s\n",tokens.token[i].token_type, tokens.token[i].str_len, tokens.token[i].str_len, tokens.token[i].str);
    }
}

Tokens tokenize(File file) {
    size_t cursor = 0;
    Token* tokens = (Token *) malloc (sizeof(Token) * MAX_TOKENS);
    Tokens final = {tokens, 0};
    
    while (cursor < file.data_len) {  
        Token token  = {};
        const char* curr_char = file.data+cursor;
        token.str = curr_char;


        switch (*curr_char) {
            case '+':
                token.token_type = TokenType_operator_add;
                token.str_len++;
                break;
            case '-':
                token.token_type = TokenType_operator_subtract;
                token.str_len++;
                break;            
            case '(':
                token.token_type = TokenType_lbrack;
                token.str_len++;
                break;
            case ')':
                token.token_type = TokenType_rbrack;
                token.str_len++;
                break;
            case '{':
                token.token_type = TokenType_lbrace;
                token.str_len++;
                break;
            case '}':
                token.token_type = TokenType_rbrace;
                token.str_len++;
                break;
            case ';':
                token.token_type = TokenType_semicolon;
                token.str_len++;
                break;
            case '=':
                if (file.data_len > cursor +1 && *(curr_char +1) == '=') {
                    token.token_type = TokenType_operator_equal;
                    token.str_len += 2;
                }
                else {
                    token.token_type = TokenType_operator_assign;
                    token.str_len++;
                }
                break;
                
            default: 

                if (isalpha(*curr_char)) {
                    token.token_type = TokenType_identifier;
                    while (isalnum(*curr_char) && (cursor + token.str_len) < file.data_len) {
                        token.str_len++;
                        curr_char++;
                    }

                    if (token.str_len == KEYWORD_IF_LEN) {
                        if (!strncmp(token.str, KEYWORD_IF, token.str_len)) 
                            token.token_type = TokenType_keyword_if;
                    }

                    else if (token.str_len == KEYWORD_INT_LEN) {
                        if (!strncmp(token.str, KEYWORD_INT, token.str_len)) 
                            token.token_type = TokenType_keyword_int;
                    }
                    else {
                        token.token_type = TokenType_identifier;
                    }

                } 
                else if (isdigit(*curr_char)) {
                    token.token_type = TokenType_number;

                    while (isdigit(*curr_char) && (cursor + token.str_len) < file.data_len) {
                        token.str_len++;
                        curr_char++;
                    }
                }
                else if (isspace(*curr_char)) {
                    cursor++;
                    continue;
                }     
                else {
                    token.token_type = TokenTypeUndefined;
                    token.str_len++;
                }
                
        
        }


        cursor += token.str_len;
        final.no_of_tokens++;

        *tokens = token;
        tokens++;

        if (final.no_of_tokens >= MAX_TOKENS) {
            printf("Max Number of Tokens reached\n");
            break;
        }

    } 


    return final;
}

