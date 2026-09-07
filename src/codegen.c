#include "codegen.h"
#include "parser.h"
#include "tokenize.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *hlt= "hlt\n";
const char *var_inititalize = " = 0\n";    
const char *pop_a = "pop A\n";    
const char *pop_b = "pop B\n";
const char *push_a = "push A\n";
const char *read_a_from_var = "lda %";
const char *read_a_from_int = "ldi A ";
const char *write_a_to_var = "sta %";
const char *new_line = "\n";
const char *perc = "%";
const char *add = "add\n";
const char *sub = "sub\n";
const char *cmp = "cmp\n";
const char *jnz = "jnz ";
char *jump_lable = "jumplable";
size_t no_of_jump_lables = 1;


Cfile new_cfile() {
    
    return (Cfile) {
        .data_section = (char *)calloc(SECTION_LEN, sizeof(char)),
        .text_section = (char *)calloc(SECTION_LEN, sizeof(char)),
        .cursor_d = 0,
        .cursor_t = 0,
        .file = NULL,
    };
}

SymbolTable new_symbol_table() {

    return (SymbolTable) {
        .symbols = {},
        .cursor = 0,
    };
} 

bool write_data(Cfile *file, const char *data, const size_t data_len) {
    if (data_len+file->cursor_d >= SECTION_LEN) {
        fprintf(stderr, "failed to write: \"%.*s\", to .data section", data_len, data);
        return false;
    }

    memcpy(file->data_section+file->cursor_d, data, data_len);
    file->cursor_d += data_len;
    return true;
}

bool write_text(Cfile *file, const char *text, const size_t text_len) {
    if (text_len+file->cursor_t >= SECTION_LEN) {
        fprintf(stderr, "failed to write: \"%.*s\", to ..text section", text_len, text);
        return false;
    }

    memcpy(file->text_section+file->cursor_t, text, text_len);
    file->cursor_t += text_len;
    return true;
}

bool write_symbol(SymbolTable *table, const Token *token) {
    if (table->cursor >= SYMBOL_TABLE_LEN) {
        fprintf(stderr, "failed to write following token to symbol table:\n");
        print_token(token);
        return false;
    }

    table->symbols[table->cursor++] = token; 
    return true;
}

bool contains_symbol(const SymbolTable *table, const Token *token) {

    for (int i = 0 ; i < table->cursor ; i++) {
        if (token->str_len != table->symbols[i]->str_len ) continue;
        if (!strncmp(token->str, table->symbols[i]->str, token->str_len)) return true;
    }

    return false;
}


bool gen_exp(Cfile *file, SymbolTable *table, const AstNode *node) {        
    
/*
    this function shall push the return value on stack which can than be utilized by callee, 
    it will work by calculating lnode and rnode such that, once it calculates rnode, 
    it will push its value on stack after which we calculate lnodes value 
    (we calculate them by basically calling this fn with respective node) 
    after that we pop one val to lnode while poping othher to rnode and then we can call add/sub
*/

    if (!node->lnode && !node->rnode) {
        if (node->token->token_type == TokenType_identifier) {
            if (!contains_symbol(table, node->token)) {
                fprintf(stderr, "following token was never declared:\n");
                print_token(node->token);
                return false;
            }

            if (
                !write_text(file, read_a_from_var, strlen(read_a_from_var))          ||
                !write_text(file, node->token->str, node->token->str_len)               ||
                !write_text(file, new_line, strlen(new_line)) ||
                !write_text(file, push_a, strlen(push_a))
            ) return false;

            return true;
        }
        else if (node->token->token_type == TokenType_number) {
            if (
                !write_text(file, read_a_from_int, strlen(read_a_from_int))          ||
                !write_text(file, node->token->str, node->token->str_len)              ||
                !write_text(file, new_line, strlen(new_line))                        ||
                !write_text(file, push_a, strlen(push_a))
            ) return false;

            return true;
        }
        else {
            fprintf(stderr, "expected identifier/number, got %u\n", node->token->token_type);
            return false;
        }

    }

    if (node->token->token_type != TokenType_operator_subtract && node->token->token_type != TokenType_operator_add ) {
        fprintf(stderr, "expected add/subtract operator, got %u\n", node->token->token_type);
        return false;
    }

    if (!gen_exp(file, table, node->rnode)) return false;    
    if (!gen_exp(file, table, node->lnode)) return false;

    const char *do_this = ((node->token->token_type == TokenType_operator_subtract ) ? sub : add) ;
    if (
        !write_text(file, pop_a, strlen(pop_a))     ||
        !write_text(file, pop_b, strlen(pop_b))     ||
        !write_text(file, do_this, strlen(do_this)) ||
        !write_text(file, push_a, strlen(push_a))
    ) return false;


    return true;
}

bool gen_init_identifier(Cfile *file, SymbolTable *table, const AstNode *node) {
    if (node->token->token_type != TokenType_operator_assign) {
        fprintf(stderr, "expected assignment operator, got %u\n", node->token->token_type);
        return false;
    }
    if (node->lnode->token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected identifier, got %u\n", node->token->token_type);
        return false;
    }

    if (!contains_symbol(table, node->lnode->token)) {
        fprintf(stderr, "following token was never declared:\n");
        print_token(node->lnode->token);
        return false;
    }   

    if (!gen_exp(file, table, node->rnode)) return false;

    if (
        !write_text(file, pop_a, strlen(pop_a))                                 ||
        !write_text(file, write_a_to_var, strlen(write_a_to_var))               ||
        !write_text(file, node->lnode->token->str, node->lnode->token->str_len)    ||
        !write_text(file, new_line, strlen(new_line))
    ) return false;


    return true;
}

bool gen_decl_identifier(Cfile *file, SymbolTable *table, const Token *token) {
    if (token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected identifier, got %u\n", token->token_type);
        return false;
    }

    if (
        !write_symbol(table, token) ||
        !write_data(file, token->str, token->str_len) ||   
        !write_data(file, var_inititalize, strlen(var_inititalize))
    ) return false;

    return true;
}

bool gen_if(Cfile *file, SymbolTable *table, const AstNode *node) {
    bool retur = true;
    if (node->lnode->token->token_type != TokenType_operator_equal) {
        fprintf(stderr, "expected \"==\", got %u\n", node->lnode->token->token_type);
    }

    if (
        !gen_exp(file, table, node->lnode->lnode) ||
        !gen_exp(file, table, node->lnode->rnode)
    ) return false;


    char ljmup_lable[64];
    size_t jump_lable_len = sprintf(ljmup_lable, "%s%lu\n", jump_lable, no_of_jump_lables);

    if (
        !write_text(file, pop_a, strlen(pop_a)) ||
        !write_text(file, pop_b, strlen(pop_b)) ||
        !write_text(file, cmp, strlen(cmp))     ||
        !write_text(file, jnz, strlen(jnz))     ||
        !write_text(file, perc, strlen(perc))     ||
        !write_text(file, ljmup_lable, jump_lable_len)
    ) return false; 

    /*size_t jnz_cursor = file->cursor_t-7;   // printf("our jnz: %.*s\n", 6, file->text_section+jnz_cursor);
    size_t initial_tcursor = file->cursor_t; */


    if (!node->rnode) 
        return true;

    AstNode *next_node = node->rnode;
    while(next_node) {
        switch (next_node->token->token_type) {
            case TokenType_identifier: 
                fprintf(stderr, "declaration is if code block is not supported for now\n");
                retur = false;
                break;
                break;
                
            case TokenType_operator_assign:
                if (!gen_init_identifier(file, table, next_node)){
                    retur = false;
                    break;
                }
                break;
            
            case TokenType_keyword_if: 
                fprintf(stderr, "nested ifs are not supported for now\n");
                retur = false;
                break;
                break;

            default:
                retur = false;
                puts("default block");
        }


        next_node = next_node->next;
    }

    if (retur) {
        size_t jump_lable_len = sprintf(ljmup_lable, "%s%lu:\n", jump_lable, no_of_jump_lables++);
        if(
            !write_text(file, ljmup_lable, jump_lable_len) ||
            !write_text(file, new_line, strlen(new_line)) 
        ) retur = false;
    }

    return retur;
}

Cfile gen_code(const Ast *ast) {
    SymbolTable table = new_symbol_table();
    Cfile file = new_cfile();

    const char *data_str = ".data\n";
    const char *text_str = ".text\nstart:\n";
    write_data(&file, data_str, strlen(data_str));
    write_text(&file, text_str, strlen(text_str));

    for (int i = 0 ; i < ast->node_block_len ; i++) {
        AstNode *node = ast->nodes[i];
        
        switch (node->token->token_type) {
            case TokenType_identifier: 
                if (!gen_decl_identifier(&file, &table, node->token)) break;

                if (node->lnode != NULL)  //we need to initialize this var
                    if (!gen_init_identifier(&file, &table, node->lnode)) break;
                
                break;
                
            case TokenType_operator_assign:
                if (!gen_init_identifier(&file, &table, node)) break;
                break;
            
            case TokenType_keyword_if: 
                if (!gen_if(&file, &table, node))break;
                break;

            default:
                puts("default block");
        }
        

    }

    write_text(&file, hlt, strlen(hlt));
    char *buf = (char *)malloc(file.cursor_d+file.cursor_t);
    memcpy(buf, file.text_section, file.cursor_t);
    memcpy(buf+file.cursor_t, file.data_section, file.cursor_d);
    file.file = buf;

    return file;
}

//free file,textsection, datasection