#include "common.h"
#include "align.h"

uintptr_t get_aligned_ptr(uintptr_t ptr, size_t aligment) {
	if(ptr % aligment != 0) {
		ptr += (aligment - (ptr % aligment));		
	}	
	return ptr;
}

int is_proper_aligment(size_t aligment) {
	if(aligment % word_size != 0) {
		return 0;
	}
	return aligment && !(aligment & (aligment - 1));
}

size_t get_block_data_offset() {
	mem_block_t block;
	uintptr_t ptr_start = (uintptr_t)&block;
	uintptr_t ptr_data = (uintptr_t)block.mb_data;
	return ptr_data - ptr_start;
}

mem_block_t *block_from_ptr(void *ptr, int offset) {
	uintptr_t block_ptr = (uintptr_t)ptr;
	block_ptr += offset;
	return (mem_block_t*)block_ptr;	
}

mem_block_t *block_from_data_ptr(void *ptr) {
	return block_from_ptr(ptr, -block_data_offset);
}

