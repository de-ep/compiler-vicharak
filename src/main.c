#include "utils.h"
#include "tokenize.h"

int main (int argc, char *argv[]) {
    if (argc < 2 ) {
        usage();
        return EXIT_FAILURE;
    }
    printf("CLI ARGS: %s %s\n", argv[0], argv[1]);

    File file = read_file(argv[1]);
    if (file.data_len <= 0)
        fprintf(stderr, "ERROR: empty file provided or could not read file\n");

    printf("CODE:\n%s\n", file.data);

    Tokens tokens = tokenize(file);

    print_tokens(tokens);

    return EXIT_SUCCESS;
}


// free tokens