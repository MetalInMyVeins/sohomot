#ifndef SUITE_H
#define SUITE_H

#include <stdint.h>

struct single_test
{
	const char* test_name;
};

struct test_suite
{
	const char* suite_name;
};

bool suites_init(void);
void suites_free(void);

#endif // SUITE_H
