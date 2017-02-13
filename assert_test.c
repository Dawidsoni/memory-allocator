#include <assert.h>
#include "common.h"
#include "assert_test.h"

void pos_blks_assert_test(mem_arena_t *arena) {
	debug_print("Pos blks assert test...\n");	
	mem_block_t *block = LIST_FIRST(&arena->ma_blocks);
	while(block != NULL) {
		uintptr_t pos = (uintptr_t)block;
		pos += abs(block->mb_size);
		block = LIST_NEXT(block, mb_list);
		if(block == NULL) {
			break;
		}
		assert((uintptr_t)block == pos);
	}
	debug_print("Pos blks assert test: OK\n");	
}

void free_blks_assert_test(mem_arena_t *arena) {
	debug_print("Free blks assert test...\n");	
	mem_block_t *block = LIST_FIRST(&arena->ma_free_blocks);
	while(block != NULL) {
		uintptr_t pos = (uintptr_t)block;
		pos += abs(block->mb_size);		
		assert(block->mb_size > 0);
		block = LIST_NEXT(block, mb_free_list);
		if(block == NULL) {
			break;
		}
		assert((uintptr_t)block != pos);
	}
	debug_print("Free blks assert test: OK\n");	
}

void arena_assert_test(mem_arena_t *arena) {
	debug_print("Arena assert test...\n");
	pos_blks_assert_test(arena);
	free_blks_assert_test(arena);			
	debug_print("Arena assert test: OK\n");	
}

void struct_assert_test() {
	debug_print("Struct assert test...\n");
	mem_arena_t *arena = LIST_FIRST(&arena_list);
	while(arena != NULL) {
		arena_assert_test(arena);
		arena = LIST_NEXT(arena, ma_list); 		
	}			
	debug_print("Struct assert test: OK\n");
}
