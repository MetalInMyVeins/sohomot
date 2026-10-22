#ifndef SOHOMOT_H
#define SOHOMOT_H

#include <stdint.h>

struct single_test
{
	const char* test_name;
};

struct test_suite
{
	const char* suite_name;
};

uint32_t get_suites_last_empty_idx();
void set_suites_last_empty_idx(uint32_t);
int64_t search_suites_idx(struct test_suite*);

#endif // SOHOMOT_H
