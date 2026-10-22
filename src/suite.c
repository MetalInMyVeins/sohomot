#include "sohomot/sohomot.h"
#include "suite.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static struct test_suite* g_suites = nullptr;
static uint32_t g_suites_last_empty_idx = 0;
static constexpr uint32_t G_SUITES_PACKET = 256;
static uint32_t g_suites_cap = G_SUITES_PACKET;

static bool compare_suite(struct test_suite* ts1, struct test_suite* ts2);
static void copy_suite(struct test_suite* dest, struct test_suite* src);
static bool suites_realloc(void);

uint32_t get_suites_last_empty_idx()
{
	return g_suites_last_empty_idx;
}

void set_suites_last_empty_idx(uint32_t idx)
{
	g_suites_last_empty_idx = idx;
}

int64_t search_suites_idx(struct test_suite* ts)
{
	uint32_t last_idx = get_suites_last_empty_idx();
	int64_t found_idx = -1;
	for (uint32_t i = 0; i < last_idx; ++i)
	{
		bool matched = compare_suite((g_suites + i), ts);
		if (matched == true)
		{
			return (int64_t)i;
		}
	}
	return found_idx;
}

bool suites_push(struct test_suite* ts)
{
	uint32_t idx = get_suites_last_empty_idx();
	if (idx == g_suites_cap)
	{
		if (suites_realloc() == false)
			return false;
	}
	*(g_suites + idx) = *ts;
	++idx;
	set_suites_last_empty_idx(idx);
	return true;
}

uint32_t get_total_suites(void)
{
	return g_suites_cap;
}

bool suites_init(void)
{
	g_suites = calloc(g_suites_cap, sizeof(*g_suites));
	if (g_suites == nullptr)
		return false;
	errno = 0;
	return true;
}

void suites_free(void)
{
	free(g_suites);
}

static bool compare_suite(struct test_suite* ts1, struct test_suite* ts2)
{
	int sn1 = strcmp(ts1->suite_name, ts2->suite_name);
	if (sn1 != 0)
		return false;
	return true;
}

static void copy_suite(struct test_suite* dest, struct test_suite* src)
{
	dest->suite_name = src->suite_name;
}

static bool suites_realloc(void)
{
	uint32_t temp_cap = g_suites_cap + G_SUITES_PACKET;
	struct test_suite* temp = calloc(temp_cap, sizeof(*g_suites));
	if (temp == nullptr)
		return false;
	errno = 0;
	g_suites_cap = temp_cap;
	uint32_t idx = get_suites_last_empty_idx();
	for (uint32_t i = 0; i < idx; ++i)
	{
		copy_suite(temp + i, g_suites + i);
	}
	free(g_suites);
	g_suites = temp;
	return true;
}
