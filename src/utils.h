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

#endif


//dealloc file buf