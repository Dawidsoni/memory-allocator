#ifndef ASSERT_TEST_H
#define ASSERT_TEST_H

#include "common.h"

void struct_assert_test();
void arena_assert_test(mem_arena_t *arena);
void free_blks_assert_test(mem_arena_t *arena);
void pos_blks_assert_test(mem_arena_t *arena);

#endif

