#ifndef SUITE_H
#define SUITE_H

#include "sohomot/sohomot.h"
#include <stdint.h>

struct sohomot_func_node
{
	sohomot_func func;
	const char* suite_name;
	const char* test_name;
	struct sohomot_func_node* next;
};

struct __attribute__((aligned(32))) test_suite
{
	const char* suite_name;
	struct sohomot_func_node* nodes_holder;
	uint32_t holder_len;
	uint32_t holder_cap;
};

bool suites_init(void);
void suites_free(void);

#endif // SUITE_H
