#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"
#include "tokenize.h"
#include <stdlib.h>

#define SECTION_LEN         1024
#define SYMBOL_TABLE_LEN    128

typedef struct {
    char *text_section;
    size_t cursor_t;
    char *data_section;
    size_t cursor_d;

    char *file;
} Cfile;

typedef struct {
    Token *symbols[SYMBOL_TABLE_LEN];
    size_t cursor;

} SymbolTable;


Cfile gen_code(const Ast *ast);

#endif