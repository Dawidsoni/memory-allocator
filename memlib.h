#ifndef MEMLIB_H
#define MEMLIB_H

#include <stdlib.h>

void *malloc(size_t size);
void *calloc(size_t count, size_t el_size);
int posix_memalign(void **mem_ptr, size_t aligment, size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

#endif
