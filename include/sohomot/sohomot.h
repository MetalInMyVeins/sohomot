#ifndef SOHOMOT_H
#define SOHOMOT_H

#include <stddef.h>

typedef struct SingleTest
{
	const char* test_name;
	struct SingleTest* next;
} SingleTest;

typedef struct TestSuite
{
	const char* suite_name;
	size_t tests_n;
	SingleTest* single_test;
	struct TestSuite* next;
} TestSuite;

#endif // SOHOMOT_H
