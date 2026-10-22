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

#endif // SOHOMOT_H
