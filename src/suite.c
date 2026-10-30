#include "suite.h"
#include "sohomot/sohomot.h"
#include <ctype.h>
#include <float.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static constexpr uint32_t PACKET_SIZE = 256;
static struct test_suite* g_suites = nullptr;
static uint32_t g_suites_cap = PACKET_SIZE;
static uint32_t g_suites_last_index = 0;

static const char* const COLOR_DEFAULT = "\033[0m";
static const char* const COLOR_GREEN = "\033[0;32m";
static const char* const COLOR_RED = "\033[0;31m";

static const char* const g_bar_equal = "[==========]";
static const char* const g_bar_minus = "[----------]";
static const char* const g_bar_run = "[ RUN      ]";
static const char* const g_bar_ok = "[       OK ]";
static const char* const g_bar_passed = "[  PASSED  ]";
static const char* const g_bar_failed = "[  FAILED  ]";

static bool g_expect_passed = true;
struct failed_node
{
	char* test_name;
	struct failed_node* next;
};
static struct failed_node* g_failed_tests = nullptr;
static struct failed_node* g_failed_tests_head = nullptr;
static uint32_t n_failed = 0;
static bool g_has_failed_test = false;

bool has_failed_test(void)
{
	return g_has_failed_test;
}

bool suites_init(void)
{
	g_suites = calloc(PACKET_SIZE, sizeof(*g_suites));
	if (g_suites == nullptr)
		return false;
	errno = 0;
	return true;
}

void suites_free(void)
{
	free(g_suites);
	g_suites = nullptr;
}

// this only invokes when g_suites_last_index == g_suites_cap.
static bool suites_realloc(void)
{
	uint32_t temp_suites_cap = g_suites_cap + PACKET_SIZE;
	struct test_suite* temp_suites = calloc(temp_suites_cap, \
			sizeof(*g_suites));
	if (temp_suites == nullptr)
		return false;
	errno = 0;
	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		temp_suites[i] = *(g_suites + i);
	}
	free(g_suites);
	g_suites = temp_suites;
	g_suites_cap = temp_suites_cap;
	return true;
}

static bool suites_holder_realloc(struct test_suite* suite)
{
	uint32_t temp_holder_cap = suite->holder_cap + PACKET_SIZE;
	struct sohomot_func_node* temp_holder = calloc(temp_holder_cap, \
			sizeof(*temp_holder));
	if (temp_holder == nullptr)
		return false;
	errno = 0;
	for (uint32_t i = 0; i < suite->holder_last_index; ++i)
	{
		temp_holder[i] = suite->nodes_holder[i];
	}
	free(suite->nodes_holder);
	suite->nodes_holder = temp_holder;
	suite->holder_cap = temp_holder_cap;
	return true;
}

static struct test_suite* search_belonging_suite(struct sohomot_func_node* node)
{
	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		if (strcmp((g_suites + i)->suite_name, node->suite_name) == 0)
			return g_suites + i;
	}
	return nullptr;
}

static struct test_suite* create_empty_suite(void)
{
	struct test_suite* temp_suite = calloc(1, sizeof(*temp_suite));
	if (temp_suite == nullptr)
		return nullptr;
	errno = 0;
	temp_suite->holder_cap = PACKET_SIZE;
	temp_suite->holder_last_index = 0;
	temp_suite->nodes_holder = calloc(temp_suite->holder_cap, \
			sizeof(*temp_suite->nodes_holder));
	if (temp_suite->nodes_holder == nullptr)
		return nullptr;
	return temp_suite;
}

static void free_empty_suite(struct test_suite* suite)
{
	free(suite);
}

static void push_node_in_suite(struct test_suite* suite, \
		struct sohomot_func_node* node)
{
	suite->nodes_holder[suite->holder_last_index] = *node;
	++suite->holder_last_index;
}

static void push_suite_in_suites(struct test_suite* suite)
{
	g_suites[g_suites_last_index] = *suite;
	++g_suites_last_index;
}

bool setup_suites(struct sohomot_func_node* nodes, uint32_t nodes_len)
{
	for (uint32_t i = 0; i < nodes_len; ++i)
	{
		struct sohomot_func_node* current_node = nodes + i;
		struct test_suite* found_suite = search_belonging_suite(current_node);
		if (found_suite)
		{
			if (found_suite->holder_last_index == found_suite->holder_cap)
			{
				if (suites_holder_realloc(found_suite) == false)
					return false;
			}
			push_node_in_suite(found_suite, current_node);
		}
		else
		{
			struct test_suite* new_suite = create_empty_suite();
			if (new_suite == nullptr)
				return false;
			errno = 0;
			if (g_suites_last_index == g_suites_cap)
			{
				if (suites_realloc() == false)
					return false;
			}
			new_suite->suite_name = current_node->suite_name;
			push_node_in_suite(new_suite, current_node);
			push_suite_in_suites(new_suite);
			free_empty_suite(new_suite);
		}
	}
	return true;
}

void clean_suites()
{
	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		free((g_suites + i)->nodes_holder);
		(g_suites + i)->holder_last_index = 0;
		(g_suites + i)->holder_cap = 0;
	}
}

void set_expect_passed(bool passed)
{
	g_expect_passed = passed;
}

bool expect_streq(const char* a, const char* b)
{
	if (a == nullptr || b == nullptr)
		return false;
	if (a == b)
		return true;

	bool status = true;
	while (*a != '\0' && *a == *b)
	{
		++a;
		++b;
	}
	if (*a != *b)
		status = false;
	return status;
}

bool expect_strcase_eq(const char* a, const char* b)
{
	if (a == nullptr || b == nullptr)
		return false;
	if (a == b)
		return true;

	bool status = true;
	while (*a != '\0' && tolower(*a) == tolower(*b))
	{
		++a;
		++b;
	}
	if (tolower(*a) != tolower(*b))
		status = false;
	return status;
}

bool expect_double_eq(const double a, const double b)
{
	if (fabs(a - b) < DBL_EPSILON)
		return true;
	return false;
}

bool expect_long_double_eq(const long double a, const long double b)
{
	if (fabsl(a - b) < LDBL_EPSILON)
		return true;
	return false;
}

bool expect_float_eq(const float a, const float b)
{
	if (fabsf(a - b) < FLT_EPSILON)
		return true;
	return false;
}

static bool feed_failed_tests(char* name)
{
	if (g_failed_tests == nullptr)
	{
		struct failed_node* temp_failed_node = calloc(1, \
				sizeof(*temp_failed_node));
		if (temp_failed_node == nullptr)
			return false;
		errno = 0;
		temp_failed_node->test_name = name;
		temp_failed_node->next = nullptr;
		g_failed_tests = temp_failed_node;
		g_failed_tests_head = temp_failed_node;
	}
	else
	{
		struct failed_node* temp_failed_node = calloc(1, \
				sizeof(*temp_failed_node));
		if (temp_failed_node == nullptr)
			return false;
		errno = 0;
		temp_failed_node->test_name = name;
		temp_failed_node->next = nullptr;
		g_failed_tests->next = temp_failed_node;
		g_failed_tests = g_failed_tests->next;
	}
	return true;
}

bool run_and_print(void)
{
	uint32_t total_suites = g_suites_last_index;
	if (total_suites == 0)
	{
		printf("No tests found.\nAbort.\n");
		errno = 0;
		return false;
	}
	uint32_t total_tests = 0;
	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		total_tests += (g_suites + i)->holder_last_index;
	}
	const char* str_suites = "suites";
	const char* str_tests = "tests";
	if (total_suites == 1)
		str_suites = "suite";
	if (total_tests == 1)
		str_tests = "test";
	printf("%s%s%s Running %u %s from %u test %s.\n", COLOR_GREEN, \
			g_bar_equal, COLOR_DEFAULT, total_tests, str_tests, \
			total_suites, str_suites);
	printf("%s%s%s Global test environment set-up.\n", COLOR_GREEN, \
			g_bar_minus, COLOR_DEFAULT);

	double global_total_time = 0;
	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		struct test_suite* current_suite = (g_suites + i);
		struct sohomot_func_node* current_holder = current_suite->nodes_holder;
		uint32_t per_suite_test = current_suite->holder_last_index;
		if (per_suite_test == 1)
			str_tests = "test";
		else
			str_tests = "tests";
		printf("%s%s%s %u %s from %s\n", COLOR_GREEN, g_bar_minus, \
				COLOR_DEFAULT, per_suite_test, str_tests, \
				current_suite->suite_name);
		double per_suite_time = 0;
		for (uint32_t j = 0; j < per_suite_test; ++j)
		{
			printf("%s%s%s %s.%s\n", COLOR_GREEN, g_bar_run, \
					COLOR_DEFAULT, current_suite->suite_name, \
					current_holder[j].test_name);
			uint64_t now_ns = get_ns();
			current_holder[j].func();
			if (g_expect_passed == true)
			{
				double temp_us = (double)(get_ns() - now_ns) / 1000;
				printf("%s%s%s %s.%s   (%.2lf us)\n", COLOR_GREEN, g_bar_ok, \
						COLOR_DEFAULT, current_suite->suite_name, \
						current_holder[j].test_name, \
					    temp_us);
				per_suite_time += temp_us;
			}
			if (g_expect_passed == false)
			{
				double temp_us = (double)(get_ns() - now_ns) / 1000;
				printf("%s%s%s %s.%s   (%.2lf us)\n", COLOR_RED, g_bar_failed, \
						COLOR_DEFAULT, current_suite->suite_name, \
						current_holder[j].test_name, \
					    temp_us);
				per_suite_time += temp_us;
				const char* suite = current_suite->suite_name;
				const char* test = current_holder[j].test_name;
				char* suite_test_concat = calloc(strlen(suite) + 1 + strlen(test), \
						sizeof(*suite_test_concat) + 1);
				if (suite_test_concat == nullptr)
					return false;
				errno = 0;
				size_t idx = 0;
				for (; idx < strlen(suite); ++idx)
				{
					suite_test_concat[idx] = suite[idx];
				}
				suite_test_concat[idx] = '.';
				++idx;
				for (size_t new_idx = 0; new_idx < strlen(test); ++new_idx)
				{
					suite_test_concat[idx] = test[new_idx];
					++idx;
				}
				suite_test_concat[idx] = '\0';
				if (feed_failed_tests(suite_test_concat) == false)
					return false;
				errno = 0;
				set_expect_passed(true);
				++n_failed;
			}
		}
		printf("%s%s%s %u %s from %s   (%.2lf us total)\n\n", COLOR_GREEN, g_bar_minus, \
				COLOR_DEFAULT, per_suite_test, str_tests, \
				current_suite->suite_name, per_suite_time);
		global_total_time += per_suite_time;
		per_suite_time = 0;
	}
	printf("%s%s%s Global test environment tear-down\n", COLOR_GREEN, \
			g_bar_minus, COLOR_DEFAULT);
	printf("%s%s%s %u %s from %u test %s ran.   (%.2lf us total)\n", COLOR_GREEN, \
			g_bar_equal, COLOR_DEFAULT, total_tests, str_tests, \
			total_suites, str_suites, global_total_time);
	uint32_t n_passed = total_tests - n_failed;
	if (n_passed == 1)
		printf("%s%s%s %u test.\n", COLOR_GREEN, g_bar_passed, \
				COLOR_DEFAULT, total_tests - n_failed);
	else
		printf("%s%s%s %u tests.\n", COLOR_GREEN, g_bar_passed, \
				COLOR_DEFAULT, total_tests - n_failed);
	if (n_failed != 0)
	{
		g_has_failed_test = true;
		if (n_failed == 1)
			printf("%s%s%s %u test, listed below:\n", COLOR_RED, \
					g_bar_failed, COLOR_DEFAULT, n_failed);
		else
			printf("%s%s%s %u tests, listed below:\n", COLOR_RED, \
					g_bar_failed, COLOR_DEFAULT, n_failed);
		struct failed_node* fn = g_failed_tests_head;
		struct failed_node* deleting_node = nullptr;
		while (fn != nullptr)
		{
			deleting_node = fn;
			printf("%s%s%s %s\n", COLOR_RED, g_bar_failed, \
					COLOR_DEFAULT, fn->test_name);
			free(fn->test_name);
			fn = fn->next;
			free(deleting_node);
		}
		printf("\n");
		if (n_failed == 1)
			printf("%u FAILED TEST\n", n_failed);
		else
			printf("%u FAILED TESTS\n", n_failed);
	}
	return true;
}

void print_int8(const char* type, const char* name, \
		const int8_t x, const char* side)
{
	printf("    %s (%s) %s -> '%c' (%d) (0x%x)\n", \
			side, type, name, x, (int32_t)x, (int32_t)x);
}

void print_uint8(const char* type, const char* name, \
		const uint8_t x, const char* side)
{
	printf("    %s (%s) %s -> '%c' (%u) (0x%x)\n", \
			side, type, name, x, (uint32_t)x, (uint32_t)x);
}

void print_int16(const char* type, const char* name, \
		const int16_t x, const char* side)
{
	printf("    %s (%s) %s -> %hd (0x%x)\n", \
			side, type, name, x, (int32_t)x);
}

void print_uint16(const char* type, const char* name, \
		const uint16_t x, const char* side)
{
	printf("    %s (%s) %s -> %hu (0x%x)\n", \
			side, type, name, x, (uint32_t)x);
}

void print_int32(const char* type, const char* name, \
		const int32_t x, const char* side)
{
	printf("    %s (%s) %s -> %d (0x%x)\n", \
			side, type, name, x, (int32_t)x);
}

void print_uint32(const char* type, const char* name, \
		const uint32_t x, const char* side)
{
	printf("    %s (%s) %s -> %u (0x%x)\n", \
			side, type, name, x, (int32_t)x);
}

void print_int64(const char* type, const char* name, \
		const int64_t x, const char* side)
{
	printf("    %s (%s) %s -> %" PRId64 " (0x%" PRIx64 ")\n", \
			side, type, name, x, x);
}

void print_uint64(const char* type, const char* name, \
		const uint64_t x, const char* side)
{
	printf("    %s (%s) %s -> %" PRIu64 " (0x%" PRIx64 ")\n", \
			side, type, name, x, x);
}

void print_long_long(const char* type, const char* name, \
		const long long x, const char* side)
{
	printf("    %s (%s) %s -> %" PRId64 " (0x%" PRIx64 ")\n", \
			side, type, name, (int64_t)x, (uint64_t)x);
}

void print_unsigned_long_long(const char* type, const char* name, \
		const unsigned long long x, const char* side)
{
	printf("    %s (%s) %s -> %" PRIu64 " (0x%" PRIx64 ")\n", \
			side, type, name, (uint64_t)x, (uint64_t)x);
}

void print_double(const char* type, const char* name, \
		const double x, const char* side)
{
	printf("    %s (%s) %s -> %.18lf\n", \
			side, type, name, x);
}

void print_long_double(const char* type, const char* name, \
		const long double x, const char* side)
{
	printf("    %s (%s) %s -> %.18Lf\n", \
			side, type, name, x);
}

void print_float(const char* type, const char* name, \
		const float x, const char* side)
{
	printf("    %s (%s) %s -> %.6f\n", \
			side, type, name, (double)x);
}

void print_str(const char* type, const char* name, \
		const char* x, const char* side)
{
	printf("    %s (%s) %s -> \"%s\"\n", \
			side, type, name, x);
}

void print_default(void)
{
	printf("Unknown type\n");
}
