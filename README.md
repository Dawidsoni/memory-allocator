# Memory allocator

## Description

Project implements the following methods:
```
void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size); 5 void free(void *ptr);
```

This project can be compiled into shared library and then used instead of standard memory allocator on Linux OS.
