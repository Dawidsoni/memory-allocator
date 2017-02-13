#include "common.h"
#include "memlib.h"
#include "arenas.h"
#include "align.h"
#include "blocks.h"

void place_block(void *addr, ssize_t size) {
	mem_block_t *block_addr = (mem_block_t*)addr;
	mem_block_t block;		
	block.mb_size = size;
	*block_addr = block;
} 

void create_main_block(mem_arena_t *arena) {
	int block_size = arena->size - sizeof(mem_arena_t);
	mem_block_t *block = (mem_block_t*)(arena + 1);
	place_block(block, block_size);
    LIST_INIT(&arena->ma_blocks); 
    LIST_INSERT_HEAD(&arena->ma_blocks, block, mb_list);    
    LIST_INIT(&arena->ma_free_blocks); 
    LIST_INSERT_HEAD(&arena->ma_free_blocks, block, mb_free_list);
}


mem_block_t *move_free_block_lcl(mem_arena_t *arena, mem_block_t *old_block, int offset) {
	mem_block_t *prev = LIST_PREV(old_block, &arena->ma_blocks, mem_block, mb_list);
	mem_block_t *free_prev = LIST_PREV(old_block, &arena->ma_free_blocks, mem_block, mb_free_list);
	LIST_REMOVE(old_block, mb_list);
	LIST_REMOVE(old_block, mb_free_list);
	ssize_t old_block_size = old_block->mb_size;
	mem_block_t *new_block = block_from_ptr(old_block, offset);
	place_block(new_block, old_block_size - offset);
	if(prev == NULL) {
		LIST_INSERT_HEAD(&arena->ma_blocks, new_block, mb_list);		
	}else {
		LIST_INSERT_AFTER(prev, new_block, mb_list);		
	}
	if(free_prev == NULL) {
		LIST_INSERT_HEAD(&arena->ma_free_blocks, new_block, mb_free_list);		
	}else {
		LIST_INSERT_AFTER(free_prev, new_block, mb_free_list);
	}
	return new_block;
}

void *malloc_full_block(mem_block_t *block) {
	LIST_REMOVE(block, mb_free_list);
	block->mb_size = -block->mb_size;
	debug_print("All. full block, addr = %lld\n", (p_ptr)block);		
	return block->mb_data;
}

void *malloc_part_block(size_t size, mem_arena_t *arena, mem_block_t *block) {
	mem_block_t *free_prev = LIST_PREV(block, &arena->ma_free_blocks, mem_block, mb_free_list);	
	LIST_REMOVE(block, mb_free_list);
	mem_block_t *rem_block = block_from_ptr(block, size);
	place_block(rem_block, block->mb_size - size);	
	LIST_INSERT_AFTER(block, rem_block, mb_list);
	if(free_prev == NULL) {
		LIST_INSERT_HEAD(&arena->ma_free_blocks, rem_block, mb_free_list);
	}else {
		LIST_INSERT_AFTER(free_prev, rem_block, mb_free_list);		
	}
	block->mb_size = -size;
	debug_print("All. part block, addr = %lld\n", (p_ptr)block);
	return block->mb_data;	
}

mem_block_t *create_splitted_block(mem_block_t *block, size_t offset) {
	mem_block_t *result = block_from_ptr(block, -offset);
	place_block(result, offset);
	LIST_INSERT_BEFORE(block, result, mb_list);
	LIST_INSERT_BEFORE(block, result, mb_free_list);
	return result;	
}

mem_block_t *align_block(mem_arena_t *arena, mem_block_t *block, size_t offset) {
	if(offset == 0) {
		return block;
	}	
	debug_print("Align block, offset: %d\n", (int)offset);		
	block = move_free_block_lcl(arena, block, offset);	
	mem_block_t *prev = LIST_PREV(block, &arena->ma_blocks, mem_block, mb_list);
	if(offset >= sizeof(mem_block_t)) {
		create_splitted_block(block, offset);
	}else if(prev != NULL) {
		prev->mb_size -= offset;
	}
	return block;
}

int is_one_free_block(mem_arena_t *arena) {
	mem_block_t *block = LIST_FIRST(&arena->ma_blocks);
	mem_block_t *next_block = LIST_NEXT(block, mb_list);
	mem_block_t *free_block = LIST_FIRST(&arena->ma_free_blocks);
	return (block == free_block && next_block == NULL);
}

void *find_block_space(size_t size, size_t aligment, mem_arena_t *arena, mem_block_t *block) {
	uintptr_t start_pos = (uintptr_t)block + block_data_offset;	
	uintptr_t offset = get_aligned_ptr(start_pos, aligment) - start_pos;
	size_t needed_size = offset + block_data_offset + size;
	if(needed_size > (size_t)block->mb_size) {
		return NULL;
	}
	malloc_arena(arena);
	block = align_block(arena, block, offset);
	needed_size -= offset;
	if(needed_size + sizeof(mem_block_t) > (size_t)block->mb_size) {
		return malloc_full_block(block);
	}else {
		return malloc_part_block(needed_size, arena, block);
	}	
}

void link_3blocks(mem_block_t *prev, mem_block_t *block, mem_block_t *next) {
	LIST_REMOVE(next, mb_free_list);
	LIST_REMOVE(next, mb_list);
	LIST_REMOVE(block, mb_list);
	prev->mb_size += (block->mb_size + next->mb_size);
}

void link_2blocks_prev(mem_block_t *prev, mem_block_t *block) {
	LIST_REMOVE(block, mb_list);
	prev->mb_size += block->mb_size;	
}

void link_2blocks_next(mem_block_t *block, mem_block_t *next) {
	LIST_REMOVE(next, mb_list);
	LIST_INSERT_BEFORE(next, block, mb_free_list);
	LIST_REMOVE(next, mb_free_list);
	block->mb_size += next->mb_size;
}

void remove_freed_block(mem_arena_t *arena, mem_block_t *block) {
	mem_block_t *prev = LIST_PREV(block, &arena->ma_blocks, mem_block, mb_list);
	LIST_REMOVE(block, mb_list);
	if(prev == NULL) {
		return;
	}
	if(prev->mb_size > 0) {
		prev->mb_size += block->mb_size;
	}else {
		prev->mb_size -= block->mb_size;
	}
}

void set_block_free(mem_arena_t *arena, mem_block_t *block) {
	mem_block_t *prev_free = block;
	do {
		prev_free = LIST_PREV(prev_free, &arena->ma_blocks, mem_block, mb_list);		
	}while(prev_free != NULL && prev_free->mb_size < 0);	
	if(prev_free == NULL) {
		LIST_INSERT_HEAD(&arena->ma_free_blocks, block, mb_free_list);
	}else {
		LIST_INSERT_AFTER(prev_free, block, mb_free_list);
	}
}

void free_block(mem_arena_t *arena, mem_block_t *block) {
	debug_print("Free block, addr: %lld\n", (p_ptr)block);	
	mem_block_t *prev = LIST_PREV(block, &arena->ma_blocks, mem_block, mb_list);
	mem_block_t *next = LIST_NEXT(block, mb_list);	
	int is_prev_free = (prev != NULL && prev->mb_size > 0);
	int is_next_free = (next != NULL && next->mb_size > 0);
	block->mb_size = -block->mb_size;
	if(is_prev_free && is_next_free) {
		link_3blocks(prev, block, next);
	}else if(is_prev_free) {
		link_2blocks_prev(prev, block);
	}else if(is_next_free) {
		link_2blocks_next(block, next);
	}else if(block->mb_size < (int)sizeof(mem_block_t)) {
		remove_freed_block(arena, block);
	}else {
		set_block_free(arena, block);		
	}
}

void add_free_block(mem_arena_t *arena, mem_block_t *block, size_t offset) {
	mem_block_t *free_block = block_from_ptr(block, offset);
	place_block(free_block, -block->mb_size - offset);
	LIST_INSERT_AFTER(block, free_block, mb_list);
	set_block_free(arena, free_block);
}

void *decrease_block(mem_arena_t *arena, mem_block_t *block, size_t size) {
	size_t old_block_size = -block->mb_size;
	mem_block_t *next = LIST_NEXT(block, mb_list);	
	int is_next_free = (next != NULL && next->mb_size > 0);
	int is_small_change = (size + sizeof(mem_block_t) > old_block_size);
	if(is_next_free) {
		move_free_block_lcl(arena, next, -(old_block_size - size));
		block->mb_size = -size;
	}else if(is_small_change == 0) {
		add_free_block(arena, block, size);
		block->mb_size = -size;
	}	
	return block->mb_data;
}

void *increase_block_lcl(mem_arena_t *arena, mem_block_t *block, mem_block_t *next, size_t size) {
	size_t offset = size + block->mb_size;
	if(next->mb_size - offset < sizeof(mem_block_t)) {
		LIST_REMOVE(next, mb_list);
		LIST_REMOVE(next, mb_free_list);		
		block->mb_size -= next->mb_size; 
	}else {
		move_free_block_lcl(arena, next, offset);	
		block->mb_size = -size;			
	}	
	return block->mb_data;
}

void *increase_block_glbl(mem_block_t *old_block, size_t size) {
	size_t old_data_size = -old_block->mb_size - block_data_offset;
	size -= block_data_offset;	
	void *result = malloc(size);
	char *new_ptr_it = (char*)result;
	char *old_ptr_it = (char*)old_block->mb_data;
	for(int i = 0; i < (int)old_data_size; i++) {
		*new_ptr_it = *old_ptr_it;
		old_ptr_it++;
		new_ptr_it++;
	}	
	free(old_block->mb_data);
	return result;
}

void *increase_block(mem_arena_t *arena, mem_block_t *block, size_t size) {
	mem_block_t *next = LIST_NEXT(block, mb_list);	
	int is_next_free = (next != NULL && next->mb_size > 0);
	size_t av_space = 0;
	if(is_next_free) {
		av_space += next->mb_size;
	}
	if(-block->mb_size + av_space >= size) {
		increase_block_lcl(arena, block, next, size);
		return block->mb_data;
	}else {
		return increase_block_glbl(block, size);
	}
}

void *resize_block(mem_arena_t *arena, void *ptr, size_t size) {
	if(is_in_arena_range(arena, ptr) == 0) {
		return NULL;
	}
	debug_print("Resizing block...\n");
	mem_block_t *block_ptr = block_from_data_ptr(ptr);
	if((int)size == -block_ptr->mb_size) {
		return block_ptr->mb_data;
	}else if((int)size < -block_ptr->mb_size) {
		return decrease_block(arena, block_ptr, size);
	}else {
		return increase_block(arena, block_ptr, size);		
	}		
}

void align_first_block(mem_arena_t *arena) {
	mem_block_t *block = LIST_FIRST(&arena->ma_blocks);
	mem_block_t *free_block = LIST_FIRST(&arena->ma_free_blocks);
	if(block != free_block) {
		return;
	}
	mem_block_t *block_pos = block_from_ptr(arena, sizeof(mem_arena_t));
	if(block_pos == free_block) {
		return;
	}
	int offset = (uintptr_t)block_pos - (uintptr_t)free_block;
	debug_print("Align first block, offset = %d\n", offset);
	move_free_block_lcl(arena, block, offset);
}

int is_valid_block_addr(mem_arena_t *arena, mem_block_t *cand_block) {
	mem_block_t *block = LIST_FIRST(&arena->ma_blocks);
	while(block != NULL) {
		if(block == cand_block && block->mb_size <= 0) {
			return 1;
		}
		block = LIST_NEXT(block, mb_list);
	}
	debug_print("Invalid block address: %lld\n", (p_ptr)cand_block);
	return 0;	
}

