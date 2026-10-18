#include "sohomot/sohomot.h"
#include <stdio.h>

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

	TestSuite* ts = &s3;

	while (ts != nullptr)
	{
		while (ts->single_test != nullptr)
		{
			printf("%s.%s\n", ts->suite_name, ts->single_test->test_name);
			ts->single_test = ts->single_test->next;
		}
		ts = ts->next;
	}
}
