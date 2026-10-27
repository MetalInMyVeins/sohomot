#include "suite.h"
#include "sohomot/sohomot.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static constexpr uint32_t PACKET_SIZE = 256;
static struct test_suite* g_suites = nullptr;
static uint32_t g_suites_cap = PACKET_SIZE;
static uint32_t g_suites_last_index = 0;

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
	if (temp_holder == false)
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
	if (g_suites_last_index == 0)
		return nullptr;
	for (uint32_t i = 0; i <= g_suites_last_index; ++i)
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
			break;
		}
		else
		{
			struct test_suite* new_suite = create_empty_suite();
			if (g_suites_last_index == g_suites_cap)
			{
				if (suites_realloc() == false)
					return false;
			}
			push_node_in_suite(new_suite, current_node);
			push_suite_in_suites(new_suite);
			free_empty_suite(new_suite);
			break;
		}
	}
	return true;
}

void clean_suites()
{
	for (uint32_t i = 0; i < g_suites_last_index; ++i)
	{
		free((g_suites + i)->nodes_holder);
	}
}
