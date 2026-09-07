#ifndef __PARSER__
#define __PARSER__

#include <stdlib.h>
#include "utils.h"
#include "tokenize.h"

#define AST_NODE_BUF_LEN        1024
#define AST_NODE_BLOCK_LEN      256


typedef struct AstNode{
    Token *token;

    struct AstNode *lnode;
    struct AstNode *rnode;

    struct AstNode *next;

} AstNode ;

typedef struct {

    AstNode *nodes[AST_NODE_BLOCK_LEN];
    size_t node_block_len;

    Tokens tokens;
    size_t token_cursor;

    AstNode *ast_node_buf;     
    size_t ast_node_buf_len;
    size_t ast_node_buf_cursor;
} Ast;

void print_node(const AstNode *node);
void print_ast (const Ast *ast);
Ast parse(Tokens tokens);

#endif