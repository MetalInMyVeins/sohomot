#ifndef SOHOMOT_H
#define SOHOMOT_H

#include <stdint.h>

struct sohomot_func_node;
typedef void (*sohomot_func)(void);
void register_sohomot_func(sohomot_func, const char* suitename, const char* testname);
bool init_arr_func_nodes(void);
void free_arr_func_nodes(void);
struct sohomot_func_node* get_nodes(void);
uint32_t get_total_nodes(void);
bool do_nodes_match(struct sohomot_func_node* n1, struct sohomot_func_node* n2);

#define TEST(Suite, Test) \
	void sohomot_##Suite##_##Test(void); \
	__attribute__((constructor)) \
	static void register_##Suite##_##Test(void) \
	{ \
		register_sohomot_func(sohomot_##Suite##_##Test, #Suite, #Test); \
	} \
	void sohomot_##Suite##_##Test(void)

#endif // SOHOMOT_H
