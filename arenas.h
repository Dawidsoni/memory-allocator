#ifndef ARENAS_H
#define ARENAS_H

mem_arena_t *create_arena(int arena_size);
mem_arena_t *create_dflt_arena();
int init_arena_list();
mem_arena_t *add_mem_arena(int arena_size);
mem_arena_t *add_dflt_mem_arena();
int remove_arena(mem_arena_t *arena);
size_t fitted_arena_size(size_t size, size_t aligment);
void malloc_arena(mem_arena_t *arena);
void *find_arena_space(size_t size, size_t aligment, mem_arena_t *arena);
int is_in_arena_range(mem_arena_t *arena, void *addr);
void free_arena(mem_arena_t *arena);
int free_arena_addr(mem_arena_t *arena, void *ptr);
void *resize_arena_block(void *ptr, size_t size);
void *find_struct_space(size_t size, size_t aligment);
void *aligned_malloc(size_t size, size_t aligment);

#endif


