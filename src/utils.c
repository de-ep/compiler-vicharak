#include "utils.h"

void usage() {
    fprintf(stderr, "USAGE: ./compiler <filename>");
}

size_t get_file_size(FILE *file) {
    size_t file_size = 0;

    fseek(file, 0, SEEK_END);

    file_size = ftell(file);

    rewind(file);

    return file_size;
}

size_t read_file_to_str(const char* pathname, char **buf) {
    size_t read = 0, file_size = 0;
    FILE* file = 0;
    bool err = false;

    file = fopen(pathname, "r");
    if (!file) {
        perror("fopen failed");
        err = true;
        goto cleanup;
    }

    file_size = get_file_size(file);
    if (!file_size)
        goto cleanup;

    *buf = (char *) malloc(file_size+1);

    read = fread(*buf, 1, file_size, file);
    (*buf)[file_size] = '\0';


cleanup: 
    if (err) {
        if (*buf)
            free(*buf);
    read = 0;
    }

    if (file)
        fclose(file);


    return read;
}

File read_file(const char *pathname) {
    File file = {NULL, 0};
    file.data_len = read_file_to_str(pathname, &file.data);

    return file;
}

bool write_to_file(const char *pathname, const char *buf, const size_t buf_len) {
    bool err = false;

    FILE *file = fopen(pathname, "w");
    if (!file) {
        perror("fopen failed");
        return false;
    }

    fwrite(buf, 1, buf_len, file);
}