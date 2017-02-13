#ifndef ALIGN_H
#define ALIGN_H

uintptr_t get_aligned_ptr(uintptr_t ptr, size_t aligment);
int is_proper_aligment(size_t aligment);
size_t get_block_data_offset();
mem_block_t *block_from_ptr(void *ptr, int offset);
mem_block_t *block_from_data_ptr(void *ptr);

#endif



