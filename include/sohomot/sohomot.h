#ifndef SOHOMOT_H
#define SOHOMOT_H

#include <stdint.h>

struct single_test;
struct test_suite;

uint32_t get_suites_last_empty_idx();
void set_suites_last_empty_idx(uint32_t);
int64_t search_suites_idx(struct test_suite*);
bool suites_push(struct test_suite* ts);
uint32_t get_total_suites(void);

typedef void (*sohomot_func)(void);
struct sohomot_func_node;
void register_sohomot_func(sohomot_func);
void free_sohomot_funcs(void);

#define TEST(Suite, Test) \
	void sohomot_##Suite##_##Test(void); \
	__attribute__((constructor)) \
	static void register_##Suite##_##Test(void) \
	{ \
		register_sohomot_func(sohomot_##Suite##_##Test); \
	} \
	void sohomot_##Suite##_##Test(void)

#endif // SOHOMOT_H
