#ifndef COMMON_H
#define COMMON_H

#include <bsd/sys/cdefs.h>
#include <bsd/sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define __uintptr_t uintptr_t

#define DEBUG 1
#ifdef DEBUG
# define debug_print(...) fprintf (stderr, __VA_ARGS__)
#else
# define debug_print(...) do {} while (0)
#endif
     
typedef struct mem_block {
	LIST_ENTRY(mem_block) mb_list;
	ssize_t mb_size;
	union {
		LIST_ENTRY(mem_block) mb_free_list;
		uintptr_t mb_data[0];
	};
} mem_block_t;

typedef struct mem_arena {
	LIST_ENTRY(mem_arena) ma_list;
	LIST_HEAD(, mem_block) ma_blocks;	
	LIST_HEAD(, mem_block) ma_free_blocks;
	size_t size;
} mem_arena_t;

typedef unsigned long long int p_ptr;

LIST_HEAD(mem_arena_list, mem_arena);

extern struct mem_arena_list arena_list; 
extern const int ARENA_PAGE_COUNT;
extern size_t word_size;
extern size_t dflt_arena_size;
extern size_t block_data_offset;
extern pthread_mutex_t mutex; 
extern pthread_mutexattr_t mutex_attr; 
extern size_t free_arena_count;

#endif
