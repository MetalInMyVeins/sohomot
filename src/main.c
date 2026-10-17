#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

int main(void)
{
	SingleTest t1 = {"Strlen", nullptr};
	SingleTest t2 = {"Strcmp", &t1};
	SingleTest t3 = {"Memcmp", &t2};
	SingleTest t4 = {"Strcat", &t3};
	SingleTest t5 = {"Isupper", nullptr};
	SingleTest t6 = {"Islower", &t5};
	SingleTest t7 = {"Printf", nullptr};
	SingleTest t8 = {"Scanf", &t7};
	TestSuite s1 = {"String", 4, &t4, nullptr};
	TestSuite s2 = {"Ctype", 2, &t6, &s1};
	TestSuite s3 = {"Stdio", 2, &t8, &s2};
	return 0;
}
