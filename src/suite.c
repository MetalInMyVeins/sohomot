#include "suite.h"
#include "sohomot/sohomot.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static constexpr uint32_t PACKET_SIZE = 256;
static struct test_suite* g_suites = nullptr;
static uint32_t g_suites_cap = PACKET_SIZE;
static uint32_t g_suites_last_index = 0;

static const char* const COLOR_DEFAULT = "\033[0m";
static const char* const COLOR_GREEN = "\033[0;32m";
// static const char* const COLOR_RED = "\033[0;31m";

static const char* const g_bar_equal = "[==========]";
static const char* const g_bar_minus = "[----------]";
static const char* const g_bar_run = "[ RUN      ]";
static const char* const g_bar_ok = "[       OK ]";
// static const char* const g_bar_passed = "[  PASSED  ]";
// static const char* const g_bar_failed = "[  FAILED  ]";

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
	struct test_suite* temp_suites = calloc(temp_suites_cap, sizeof(*g_suites));
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
	struct sohomot_func_node* temp_holder = calloc(temp_holder_cap, sizeof(*temp_holder));
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
	temp_suite->nodes_holder = calloc(temp_suite->holder_cap, sizeof(*temp_suite->nodes_holder));
	if (temp_suite->nodes_holder == nullptr)
		return nullptr;
	return temp_suite;
}

static void free_empty_suite(struct test_suite* suite)
{
	free(suite);
}

static void push_node_in_suite(struct test_suite* suite, struct sohomot_func_node* node)
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

void print_suites_status(void)
{
	uint32_t total_suites = g_suites_last_index;
	if (total_suites == 0)
	{
		printf("No tests found.\nAbort.\n");
		return;
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
	printf("%s%s%s Running %u %s from %u test %s.\n", COLOR_GREEN, g_bar_equal, COLOR_DEFAULT, total_tests, str_tests, total_suites, str_suites);
	printf("%s%s%s Global test environment set-up.\n", COLOR_GREEN, g_bar_minus, COLOR_DEFAULT);

	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		struct test_suite* current_suite = (g_suites + i);
		struct sohomot_func_node* current_holder = current_suite->nodes_holder;
		uint32_t per_suite_test = current_suite->holder_last_index;
		if (per_suite_test == 1)
			str_tests = "test";
		else
			str_tests = "tests";
		printf("%s%s%s %u %s from %s\n", COLOR_GREEN, g_bar_minus, COLOR_DEFAULT, per_suite_test, str_tests, current_suite->suite_name);
		for (uint32_t j = 0; j < per_suite_test; ++j)
		{
			printf("%s%s%s %s.%s\n", COLOR_GREEN, g_bar_run, COLOR_DEFAULT, current_suite->suite_name, current_holder[j].test_name);
			current_holder[j].func();
			printf("%s%s%s %s.%s\n", COLOR_GREEN, g_bar_ok, COLOR_DEFAULT, current_suite->suite_name, current_holder[j].test_name);
		}
		printf("%s%s%s %u %s from %s\n\n", COLOR_GREEN, g_bar_minus, COLOR_DEFAULT, per_suite_test, str_tests, current_suite->suite_name);
	}
}
