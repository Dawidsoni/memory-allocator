#include "common.h"
#include "dump_struct.h"

void dump_block(mem_block_t  *block) {
	debug_print("Block address: %lld, ", (p_ptr)block);
	debug_print("size = %d, ", abs((int)block->mb_size));
	debug_print("is_free = %d\n", block->mb_size > 0);				
}

void dump_blocks(mem_arena_t *arena) {
	debug_print("Arena blocks:\n");
	mem_block_t *block = LIST_FIRST(&arena->ma_blocks);
	while(block != NULL) {
		dump_block(block);
		block = LIST_NEXT(block, mb_list);
	}
}

void dump_free_blocks(mem_arena_t *arena) {
	mem_block_t *block = LIST_FIRST(&arena->ma_free_blocks);
	if(block == NULL) {
		debug_print("No free blocks\n");						
		return;
	}
	debug_print("Arena free blocks:\n");				
	while(block != NULL) {
		dump_block(block);
		block = LIST_NEXT(block, mb_free_list);
	}
}

void dump_arena(mem_arena_t *arena) {
	debug_print("Arena address: %lld, ", (p_ptr)arena);		
	debug_print("size: %d\n", (int)arena->size);	
	dump_blocks(arena);
	dump_free_blocks(arena);
	debug_print("\n");
}

void dump_struct() {
	mem_arena_t *arena = LIST_FIRST(&arena_list);
	while(arena != NULL) {
		dump_arena(arena);
		arena = LIST_NEXT(arena, ma_list); 		
	}		
	debug_print("\n\n");
}
