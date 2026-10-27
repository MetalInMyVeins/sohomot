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
	uint32_t holder_cap;
	uint32_t holder_last_index;
};

bool suites_init(void);
void suites_free(void);
bool setup_suites(struct sohomot_func_node* nodes, uint32_t nodes_len);
void clean_suites();

#endif // SUITE_H
