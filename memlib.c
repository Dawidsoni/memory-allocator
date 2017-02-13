#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include "common.h"
#include "arenas.h"
#include "init.h"
#include "align.h"
#include "memlib.h"

struct mem_arena_list arena_list; 	 	
const int ARENA_PAGE_COUNT = 4;
size_t word_size;
size_t dflt_arena_size;
size_t block_data_offset;
pthread_mutex_t mutex; 
pthread_mutexattr_t mutex_attr; 
size_t free_arena_count;

void *malloc(size_t size) {
	pthread_mutex_lock(&mutex);
	if(init_if_new() == -1) {
		return NULL;
	}
	void *result = aligned_malloc(size, word_size);
	pthread_mutex_unlock(&mutex);
	return result;
}

void *calloc(size_t count, size_t el_size) {
	pthread_mutex_lock(&mutex);
	if(init_if_new() == -1) {
		return NULL;
	}	
	size_t size = count * el_size;
	void *result = aligned_malloc(size, word_size);
	if(result == NULL) {
		return NULL;
	}
	init_interval(result, size);
	pthread_mutex_unlock(&mutex);
	return result;
}

int posix_memalign(void **mem_ptr, size_t aligment, size_t size) {	
	pthread_mutex_lock(&mutex);		
	if(init_if_new() == -1) {
		return ENOMEM;
	}else if(is_proper_aligment(aligment) == 0) {
		return EINVAL;
	}
	*mem_ptr = aligned_malloc(size, aligment);
	pthread_mutex_unlock(&mutex);
	if(*mem_ptr == NULL) {
		return ENOMEM;
	}
	return 0;	
}

void free(void *ptr) {
	if(ptr == NULL) {
		return;
	}
	pthread_mutex_lock(&mutex);	
	mem_arena_t *arena = LIST_FIRST(&arena_list);
	while(arena != NULL) {
		if(free_arena_addr(arena, ptr) != -1) {
			return;
		}
		arena = LIST_NEXT(arena, ma_list);
	}
	pthread_mutex_unlock(&mutex);
}

void *realloc(void *ptr, size_t size) {
	pthread_mutex_lock(&mutex);
	if(ptr == NULL) {
		return malloc(size);
	}else if(size == 0) {
		free(ptr);
		return NULL;
	}else {
		return resize_arena_block(ptr, size + block_data_offset);
	}
	pthread_mutex_unlock(&mutex);
}

