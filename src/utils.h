#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char *data;
    size_t data_len;

} File;

void usage();
File read_file(const char *pathname);
bool write_to_file(const char *pathname, const char *buf, const size_t buf_len);

#endif


//dealloc file buf