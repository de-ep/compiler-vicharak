#include "parser.h"
#include "tokenize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AstNode *new_astnode (Ast *ast) {

    if (ast->ast_node_buf_cursor >= ast->ast_node_buf_len) {
            fprintf(stderr, "cant create more ast nodes\n");
            return NULL;
        }

    return ast->ast_node_buf + ast->ast_node_buf_cursor++;
}

Ast new_ast (Tokens tokens) {

    return (Ast) {
        .nodes = {},
        .tokens = tokens,
        .token_cursor = 0,
        .ast_node_buf = (AstNode *)calloc(sizeof(AstNode), AST_NODE_BUF_LEN),
        .ast_node_buf_len = AST_NODE_BUF_LEN,
        .ast_node_buf_cursor = 0,
    };
}

void undo_astnode_allocation(Ast *ast, size_t intial_node_buf_cursor) {
        memset(
            ast->ast_node_buf+ast->ast_node_buf_cursor, 
            0, 
            (ast->ast_node_buf_cursor- intial_node_buf_cursor) * sizeof(AstNode)
        );
        ast->ast_node_buf_cursor = intial_node_buf_cursor;
}

bool insert_block_node(Ast *ast, AstNode *node) {
    if (ast->node_block_len >= ast->ast_node_buf_len)
        return false;

    ast->nodes[ast->node_block_len++] = node;

    return true;
}


AstNode *parse_operation(Ast *ast) {        //validate the end of operation
    if (ast->token_cursor+3 >= ast->tokens.no_of_tokens) {
        fprintf(stderr, "ran out of tokens\n");
        return NULL;
    }
    size_t i = 0;

    Token *cur_token = &ast->tokens.token[ast->token_cursor+i++];
    Token *next_token = &ast->tokens.token[ast->token_cursor+i++];
    Token *next_next_token = &ast->tokens.token[ast->token_cursor+i++];
    ast->token_cursor += i;

    if (cur_token->token_type != TokenType_number && cur_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier/number, got: %.*s\n", cur_token->str_len, cur_token->str);
        return NULL;
    }
    if (next_token->token_type != TokenType_operator_add && next_token->token_type != TokenType_operator_subtract) {
        fprintf(stderr, "expected: \"+\" or \"-\" operator, got: %.*s\n", next_token->str_len, next_token->str);
        return NULL;
    }    
    if (next_next_token->token_type != TokenType_number && next_next_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier/number, got: %.*s\n", next_next_token->str_len, next_next_token->str);
        return NULL;
    }
    
    AstNode *node = new_astnode(ast);
    AstNode *lnode = new_astnode(ast);
    AstNode *rnode = new_astnode(ast);
    
    if (!node || !lnode || !rnode) return NULL;
    lnode->token = cur_token;
    rnode->token = next_next_token;
    node->token = next_token;
    node->lnode = lnode;
    node->rnode = rnode;

    cur_token = &ast->tokens.token[ast->token_cursor];
    if (
        cur_token->token_type == TokenType_semicolon        || 
        cur_token->token_type == TokenType_rbrack           ||
        cur_token->token_type == TokenType_operator_equal 
    ) return node;

    if (cur_token->token_type == TokenType_operator_add || cur_token->token_type == TokenType_operator_subtract ) {
        ast->token_cursor--;
        AstNode *node2 = parse_operation(ast);
        node2->lnode = node;
        return node2;
    }

    fprintf(stderr, "Unexpected input: %.*s\n", cur_token->str_len, cur_token->str);

    return NULL; 
}

AstNode *parse_assign(Ast *ast) {
    size_t intial_node_buf_cursor = ast->ast_node_buf_cursor;
    if (ast->token_cursor+3 >= ast->tokens.no_of_tokens) {
        fprintf(stderr, "ran out of tokens\n");
        return NULL;
    }
    size_t i = 0;

    Token *cur_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_next_token = &ast->tokens.token[ast->token_cursor++];


    if (cur_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier, got: %.*s\n", cur_token->str_len, cur_token->str);
        return NULL;
    }
    if (next_token->token_type != TokenType_operator_assign) {
        fprintf(stderr, "expected: \"=\", got: %.*s\n", next_token->str_len, next_token->str);
        return NULL;
    }
    if (next_next_token->token_type != TokenType_number && next_next_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier/number, got: %.*s\n", next_next_token->str_len, next_next_token->str);
        return NULL;
    }

    AstNode *node = new_astnode(ast);
    AstNode *lnode = new_astnode(ast);
    AstNode *rnode = new_astnode(ast);
    
    if (!node || !lnode || !rnode) return NULL;

    lnode->token = cur_token;
    rnode->token = next_next_token;
    node->token = next_token;
    node->lnode = lnode;
    node->rnode = rnode;

    cur_token = &ast->tokens.token[ast->token_cursor];
    if(cur_token->token_type == TokenType_operator_add || cur_token->token_type == TokenType_operator_subtract) {
        ast->token_cursor--;
        AstNode *node2 = parse_operation(ast);
        node->rnode = node2;
    }

    cur_token = &ast->tokens.token[ast->token_cursor++];
    if (cur_token->token_type != TokenType_semicolon) {
        fprintf(stderr, "expected: \";\", got: %.*s\n", cur_token->str_len, cur_token->str);
        undo_astnode_allocation(ast, intial_node_buf_cursor);
        return NULL;
    }

    return node;
}

AstNode *parse_declaration(Ast *ast) {
    if (ast->token_cursor+2 >= ast->tokens.no_of_tokens)  {
        fprintf(stderr, "ran out of tokens\n");
        return NULL;
    }
    size_t i = 0;

    Token *cur_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_next_token = &ast->tokens.token[ast->token_cursor++];
    
    if (cur_token->token_type != TokenType_keyword_int) {
        fprintf(stderr, "expected: \"int\",got: %.*s\n", cur_token->str_len, cur_token->str);
        return NULL;
    }

    if (next_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier ,got: %.*s\n", next_token->str_len, next_token->str);
        return NULL;
    }

    AstNode *lnode = NULL;
    if (next_next_token->token_type == TokenType_operator_assign) {
        ast->token_cursor -= 2;
        //print_token(&ast->tokens.token[ast->token_cursor]);
        lnode = parse_assign(ast);
    }
    
    else if (next_next_token->token_type != TokenType_semicolon) {
        fprintf(stderr, "expected: \";\" ,got: %.*s\n", next_next_token->str_len, next_next_token->str);
        return NULL;
    }

    AstNode *node = new_astnode(ast);
    if (!node) return NULL;

    node->token = next_token;
    node->lnode = lnode;

    return node;
}

AstNode *parse_equal(Ast *ast) {    //check if equality ends with )
    if (ast->token_cursor+2 >= ast->tokens.no_of_tokens)  {
        fprintf(stderr, "ran out of tokens\n");
        return NULL;
    }
    size_t i = 0;

    Token *cur_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_next_token = &ast->tokens.token[ast->token_cursor++];

    if (cur_token->token_type != TokenType_number && cur_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier/number, got: %.*s\n", cur_token->str_len, cur_token->str);
        return NULL;
    }
    if (next_token->token_type != TokenType_operator_equal) {
        fprintf(stderr, "expected: \"==\", got: %.*s\n", next_token->str_len, next_token->str);
        return NULL;
    }
    if (next_next_token->token_type != TokenType_number && next_next_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier/number, got: %.*s\n", next_next_token->str_len, next_next_token->str);
        return NULL;
    }

    AstNode *node = new_astnode(ast);
    AstNode *lnode = new_astnode(ast);
    AstNode *rnode = new_astnode(ast);
    
    if (!node || !lnode || !rnode) return NULL;

    lnode->token = cur_token;
    rnode->token = next_next_token;
    node->token = next_token;
    node->lnode = lnode;
    node->rnode = rnode;    

    return node;
}

AstNode *parse_if(Ast *ast) {    
    size_t intial_node_buf_cursor = ast->ast_node_buf_cursor;
    if (ast->token_cursor+3 >= ast->tokens.no_of_tokens)  {
        fprintf(stderr, "ran out of tokens\n");
        return NULL;
    }
    size_t i = 0;

    Token *cur_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_token = &ast->tokens.token[ast->token_cursor++];
    Token *next_next_token = &ast->tokens.token[ast->token_cursor];

    if (cur_token->token_type != TokenType_keyword_if) {
        fprintf(stderr, "expected: \"if\", got: %.*s\n", cur_token->str_len, cur_token->str);
        return NULL;
    }
    if (next_token->token_type != TokenType_lbrack) {
        fprintf(stderr, "expected: \"(\", got: %.*s\n", cur_token->str_len, cur_token->str);
        return NULL;
    }
    if (next_next_token->token_type != TokenType_number && next_next_token->token_type != TokenType_identifier) {
        fprintf(stderr, "expected: identifier/number, got: %.*s\n", next_next_token->str_len, next_next_token->str);
        return NULL;
    }

    AstNode *node = new_astnode(ast);
    AstNode *lnode = new_astnode(ast);
    AstNode *rnode = new_astnode(ast);
    if (!node || !lnode || !rnode) return NULL;

    rnode->token = next_next_token;
    node->token = cur_token;
    node->lnode = parse_equal(ast);
    
    cur_token = &ast->tokens.token[ast->token_cursor++];
    if (cur_token->token_type != TokenType_rbrack) {
        fprintf(stderr, "expected: \")\", got: %.*s\n", cur_token->str_len, cur_token->str);
        undo_astnode_allocation(ast, intial_node_buf_cursor);
        return NULL;
    }


    cur_token = &ast->tokens.token[ast->token_cursor++];
    if (cur_token->token_type != TokenType_lbrace) {
        fprintf(stderr, "expected: \"{\", got: %.*s\n", cur_token->str_len, cur_token->str);
        undo_astnode_allocation(ast, intial_node_buf_cursor);
        return NULL;
    }

    cur_token = &ast->tokens.token[ast->token_cursor];

    bool first_node = true;
    AstNode *tmp_node = NULL;
    AstNode *prev_node = NULL;
    while (cur_token->token_type != TokenType_rbrace) {
        cur_token = &ast->tokens.token[ast->token_cursor];
        switch (cur_token->token_type) {
        
            case TokenType_keyword_int :
                fprintf(stderr, "declaration under if codeblocks are not supported for now\n");
                ast->token_cursor++;
                cur_token = &ast->tokens.token[ast->token_cursor];
                continue;
            /*
                tmp_node = parse_declaration(ast);
                break;*/
            case TokenType_identifier : 
                tmp_node = parse_assign(ast);
                break;
            case TokenType_keyword_if : 
                fprintf(stderr, "nested ifs are not supported for now\n");
                ast->token_cursor++;
                cur_token = &ast->tokens.token[ast->token_cursor];
                continue;
            case TokenType_rbrace :
                break;
                break;
            default:
                fprintf(stderr, "Unexpected token: %.*s\n", cur_token->str_len, cur_token->str);
                ast->token_cursor++;
                cur_token = &ast->tokens.token[ast->token_cursor];
                continue;
        }
    
        if (!tmp_node) continue; 
        /*       
        if (first_node) {
            node->rnode = tmp_node;
        }
        else {
            Plnode->lnode = tmp_node;
        }
        Plnode = tmp_node;
        while (Plnode->lnode != NULL) {
            Plnode = Plnode->lnode;
        }
        */

        if (first_node) {
            node->rnode = tmp_node;
            first_node = false;
        }
        else {
            prev_node->next = tmp_node;
        }
        prev_node = tmp_node;
        
        cur_token = &ast->tokens.token[ast->token_cursor];
    }
    

    if (cur_token->token_type != TokenType_rbrace) {
        fprintf(stderr, "expected: \"}\", got: %.*s\n", cur_token->str_len, cur_token->str);
        undo_astnode_allocation(ast, intial_node_buf_cursor);
        return NULL;
    }
    ast->token_cursor++;

    return node;
}

void print_node(const AstNode *node) {
    print_token(node->token);
    (node->lnode == NULL) ? puts("NULL") : print_node(node->lnode);
    (node->rnode == NULL) ? puts("NULL") : print_node(node->rnode);
    (node->next == NULL) ? puts("NULL") : print_node(node->next);

}

void print_ast (const Ast *ast) {
    puts("\nAST:");
    printf("node block len: %zu\n", ast->node_block_len);
    for (size_t i = 0 ; i < ast->node_block_len ; i++) {
        print_node(ast->nodes[i]);
    }

}



Ast parse(Tokens tokens) {
    Ast ast = new_ast(tokens);
    size_t node_block_i = 0;

    while (ast.token_cursor < ast.tokens.no_of_tokens && ast.node_block_len < ast.ast_node_buf_len) {
        Token *token = &ast.tokens.token[ast.token_cursor];
        AstNode *node = NULL;

        switch (token->token_type) {
        
            case TokenType_keyword_int :
                node = parse_declaration(&ast);
                break;
            case TokenType_identifier : 
                node = parse_assign(&ast);
                break;
            case TokenType_keyword_if : 
                node = parse_if(&ast);
                break;
            default:
                fprintf(stderr, "Unexpected token: %.*s\n", token->str_len, token->str);
                ast.token_cursor++;

                
       }
    
       if (!node) continue; 
       insert_block_node(&ast, node);
    }

    ast.token_cursor = 0;

    return ast;
}