#include "codegen.h"
#include "parser.h"
#include "utils.h"
#include "tokenize.h"
#include <stdio.h>

int main (int argc, char *argv[]) {
    if (argc < 2 ) {
        usage();
        return EXIT_FAILURE;
    }
    printf("CLI ARGS: %s %s\n", argv[0], argv[1]);

    File file = read_file(argv[1]);
    if (file.data_len <= 0)
        fprintf(stderr, "ERROR: empty file provided or could not read file\n");

    //printf("CODE:\n%s\n", file.data);

    Tokens tokens = tokenize(file);

    //print_tokens(tokens);

    Ast ast = parse(tokens);
    //print_ast(&ast);

    Cfile cfile = gen_code(&ast);
    //printf("file: %.*s", cfile.cursor_d+cfile.cursor_t, cfile.file);

    write_to_file("out.txt", cfile.file, cfile.cursor_d+cfile.cursor_t);

    free(cfile.data_section);
    free(cfile.text_section);
    free(file.data);
    free(cfile.file);
    free(tokens.token);
    free(ast.ast_node_buf);


    return EXIT_SUCCESS;
}


// free tokens