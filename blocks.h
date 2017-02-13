#ifndef BLOCKS_H
#define BLOCKS_H

void place_block(void *addr, ssize_t size);
void create_main_block(mem_arena_t *arena);
mem_block_t *move_free_block_lcl(mem_arena_t *arena, mem_block_t *old_block, int offset);
void *malloc_full_block(mem_block_t *block);
void *malloc_part_block(size_t size, mem_arena_t *arena, mem_block_t *block);
mem_block_t *create_splitted_block(mem_block_t *block, size_t offset);
mem_block_t *align_block(mem_arena_t *arena, mem_block_t *block, size_t offset);
int is_one_free_block(mem_arena_t *arena);
void *find_block_space(size_t size, size_t aligment, mem_arena_t *arena, mem_block_t *block);
void link_3blocks(mem_block_t *prev, mem_block_t *block, mem_block_t *next);
void link_2blocks_prev(mem_block_t *prev, mem_block_t *block);
void link_2blocks_next(mem_block_t *block, mem_block_t *next);
void remove_freed_block(mem_arena_t *arena, mem_block_t *block);
void set_block_free(mem_arena_t *arena, mem_block_t *block);
void free_block(mem_arena_t *arena, mem_block_t *block);
void add_free_block(mem_arena_t *arena, mem_block_t *block, size_t offset);
void *decrease_block(mem_arena_t *arena, mem_block_t *block, size_t size);
void *increase_block_lcl(mem_arena_t *arena, mem_block_t *block, mem_block_t *next, size_t size);
void *increase_block_glbl(mem_block_t *old_block, size_t size);
void *increase_block(mem_arena_t *arena, mem_block_t *block, size_t size);
void *resize_block(mem_arena_t *arena, void *ptr, size_t size);
void align_first_block(mem_arena_t *arena);
int is_valid_block_addr(mem_arena_t *arena, mem_block_t *cand_block);

#endif

