#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "arenas.h"
#include "align.h"
#include "init.h"

int init_arena_list() {
	debug_print("Initializing arena list...\n");
    LIST_INIT(&arena_list);   	
    mem_arena_t *arena_head = create_dflt_arena(); 
    if(arena_head == NULL) {
		return -1;
	}
    LIST_INSERT_HEAD(&arena_list, arena_head, ma_list);	
    free_arena_count = 1;
    return 0;
}

void init_mutex() {
	debug_print("Initializing mutex...\n");	
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &mutex_attr);
}

int init_if_new() {
	if(LIST_EMPTY(&arena_list) == 0) {
		return 0;
	}
	debug_print("Initializing struct...\n");
	dflt_arena_size = getpagesize() * ARENA_PAGE_COUNT;
	debug_print("Default arena size = %d\n", (int)dflt_arena_size);
    word_size = sizeof(void*);		
 	debug_print("Machine word size = %d\n", (int)word_size);  
 	block_data_offset = get_block_data_offset();
 	debug_print("Block data offset = %d\n", (int)block_data_offset);  
 	init_mutex();	
    return init_arena_list();
}

void init_interval(void *ptr, size_t size) {
	char *it_ptr = (char*)ptr;
	for(int i = 0; i < (int)size; i++) {
		*it_ptr = 0;
		it_ptr++;
	}
}
