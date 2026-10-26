#ifndef SOHOMOT_H
#define SOHOMOT_H

#include <stdint.h>

// struct test_suite;
//
// uint32_t get_suites_last_empty_idx();
// void set_suites_last_empty_idx(uint32_t);
// int64_t search_suites_idx(struct test_suite*);
// bool suites_push(struct test_suite* ts);
// uint32_t get_total_suites(void);

struct sohomot_func_node;
typedef void (*sohomot_func)(void);
void register_sohomot_func(sohomot_func, const char* suitename, const char* testname);
bool init_arr_func_nodes(void);
void free_arr_func_nodes(void);
// struct sohomot_func_node* get_sohomot_func_node(void);
// uint32_t get_sohomot_func_node_len(void);
// struct sohomot_func_node* get_arr_func_nodes(void);
// void setup_suites(struct sohomot_func_node* arr_func_nodes, uint32_t node_len);

#define TEST(Suite, Test) \
	void sohomot_##Suite##_##Test(void); \
	__attribute__((constructor)) \
	static void register_##Suite##_##Test(void) \
	{ \
		register_sohomot_func(sohomot_##Suite##_##Test, #Suite, #Test); \
	} \
	void sohomot_##Suite##_##Test(void)

#endif // SOHOMOT_H
