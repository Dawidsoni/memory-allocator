#include <sys/mman.h>
#include <sys/types.h>
#include "common.h"
#include "blocks.h"
#include "arenas.h"
#include "align.h"

mem_arena_t *create_arena(int arena_size) {
	int prot = PROT_READ | PROT_WRITE;
	int flags = MAP_ANONYMOUS | MAP_PRIVATE;
	mem_arena_t *result = mmap(NULL, arena_size, prot, flags, -1, 0);
	if(result == MAP_FAILED) {
		return NULL;
	}
	result->size = arena_size;	
	create_main_block(result);
	debug_print("Creating arena, size = %d\n", (int)result->size);		
	return result;
}

mem_arena_t *create_dflt_arena() {
	return create_arena(dflt_arena_size);
}

mem_arena_t *add_mem_arena(int arena_size) {
	debug_print("Adding new arena...\n");	
	mem_arena_t *result = create_arena(arena_size);
	if(result == NULL) {
		return NULL;
	}
	LIST_INSERT_HEAD(&arena_list, result, ma_list);
	free_arena_count++;
	return result;
}

mem_arena_t *add_dflt_mem_arena() {
	return add_mem_arena(dflt_arena_size);
}

int remove_arena(mem_arena_t *arena) {
	debug_print("Removing arena...\n");		
	LIST_REMOVE(arena, ma_list);
	return munmap(arena, arena->size);
}

size_t fitted_arena_size(size_t size, size_t aligment) {
	debug_print("Fitting arena (%d, %d)\n", (int)size, (int)aligment);		
	size_t prefix_size = sizeof(mem_arena_t) + block_data_offset;
	prefix_size = get_aligned_ptr(prefix_size, aligment);
	prefix_size += size;	
	if(prefix_size <= dflt_arena_size) {
		debug_print("Fitted arena: %d\n", (int)dflt_arena_size);					
		return dflt_arena_size;
	}
	debug_print("Fitted arena: %d\n", (int)prefix_size);			
	return prefix_size;
}

void malloc_arena(mem_arena_t *arena) {
	if(is_one_free_block(arena) == 0) {
		return;
	}
	debug_print("Malloc arena...\n");
	free_arena_count--;		
}


void *find_arena_space(size_t size, size_t aligment, mem_arena_t *arena) {
	mem_block_t* block = LIST_FIRST(&arena->ma_free_blocks);
	while(block != NULL) {
		void *addr = find_block_space(size, aligment, arena, block);
		if(addr != NULL) {
			return addr;
		}
		block = LIST_NEXT(block, mb_free_list);
	}
	return NULL;
}

int is_in_arena_range(mem_arena_t *arena, void *addr) {
	uintptr_t start_range = (uintptr_t)arena;
	uintptr_t end_range = start_range + arena->size;
	uintptr_t point = (uintptr_t)addr;
	if(start_range < end_range) {
		return (point >= start_range && point <= end_range);
	}
	return (point >= start_range || point <= end_range);
}

void free_arena(mem_arena_t *arena) {
	if(is_one_free_block(arena) == 0) {
		return;
	}
	debug_print("Free arena...\n");
	free_arena_count++;
	if(free_arena_count > 1) {
		remove_arena(arena);
	}
}

int free_arena_addr(mem_arena_t *arena, void *ptr) {
	if(is_in_arena_range(arena, ptr) == 0) {
		return -1;
	}
	mem_block_t *block_ptr = block_from_data_ptr(ptr);
	if(is_valid_block_addr(arena, block_ptr) == 0) {
		return -1;
	}
	free_block(arena, block_ptr);	
	align_first_block(arena);
	free_arena(arena);
	return 0;	
}


void *resize_arena_block(void *ptr, size_t size) {
	mem_arena_t *arena = LIST_FIRST(&arena_list);
	while(arena != NULL) {
		void *result = resize_block(arena, ptr, size);
		if(result != NULL) {
			return result;
		}
		arena = LIST_NEXT(arena, ma_list);
	}	
	return NULL;
}

void *find_struct_space(size_t size, size_t aligment) {
	mem_arena_t *arena = LIST_FIRST(&arena_list);
	while(arena != NULL) {
		void *addr = find_arena_space(size, aligment, arena);
		if(addr != NULL) {
			return addr;
		}
		arena = LIST_NEXT(arena, ma_list);
	}
	return NULL;
}

void *aligned_malloc(size_t size, size_t aligment) {
	void *result = find_struct_space(size, aligment);
	if(result != NULL) {
		return result;
	}
	size_t arena_size = fitted_arena_size(size, aligment);
	mem_arena_t *arena = add_mem_arena(arena_size);
	if(arena == NULL) {
		return NULL;
	}
	return find_arena_space(size, aligment, arena);
}



