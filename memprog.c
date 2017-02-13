#include <time.h>
#include "memlib.h"
#include "dump_struct.h"
#include "assert_test.h"

const int SIZE = 30000;
void *ptr_list[30000];
int ptr_size[30000];

void allocate_all() {
	for(int i = 0; i < SIZE; i++) {
		if(rand() % 100 == 0) {
			ptr_size[i] = rand() % 16000 + 16000;
		}else {
			ptr_size[i] = rand() % 100 + 1;			
		}		
		if(rand() % 2 == 0) {
			ptr_list[i] = calloc(ptr_size[i], 1);
		}else {
			posix_memalign(&ptr_list[i], 128, ptr_size[i]);			
		}		
	}	
}

void reallocate(int start, int jump) {
	for(int i = start; i < SIZE; i += jump) {
		ptr_size[i] = rand() % 200 + 1;
		ptr_list[i] = realloc(ptr_list[i], ptr_size[i]);
	}	
}

void reallocate_all() {
	reallocate(1, 3);
	reallocate(0, 3);
	reallocate(2, 3);
}

void free_ptr(int start, int jump) {
	for(int i = start; i < SIZE; i += jump) {
		free(ptr_list[i]);
	}	
}

void free_ptr_all() {
	free_ptr(2, 3);
	free_ptr(1, 3);
	free_ptr(0, 3);
}

void walk(int num) {
	char* ptr = (char*)ptr_list[num];
	for(int i = 0; i < ptr_size[num]; i++) {
		*ptr = 'a';
		ptr++;
	}
}

void walk_all() {
	for(int i = 0; i < SIZE; i++) {
		walk(i);
	}		
}

int main() { 
	srand(time(NULL));
	allocate_all();
	struct_assert_test();	
	walk_all();
	struct_assert_test();		
	reallocate_all();
	struct_assert_test();
	walk_all();
	struct_assert_test();
	free_ptr_all();
	struct_assert_test();
	dump_struct();
	return 0;
}
