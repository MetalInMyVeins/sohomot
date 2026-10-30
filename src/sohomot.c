#include "sohomot/sohomot.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sohomot_func_node
{
	sohomot_func func;
	const char* suite_name;
	const char* test_name;
	struct sohomot_func_node* next;
};

static struct sohomot_func_node* g_sfn = nullptr;
static struct sohomot_func_node* g_sfn_head = nullptr;
static uint32_t g_node_len = 0;
static struct sohomot_func_node* arr_func_nodes = nullptr;

void register_sohomot_func(sohomot_func func, \
		const char* suitename, const char* testname)
{
	if (g_sfn == nullptr)
	{
		g_sfn = calloc(1, sizeof(*g_sfn));
		if (g_sfn == nullptr)
		{
			fprintf(stderr, "sohomot: failed to register test\n");
			return;
		}

		g_sfn->func = func;
		g_sfn->suite_name = suitename;
		g_sfn->test_name = testname;
		g_sfn->next = nullptr;
		g_sfn_head = g_sfn;
		++g_node_len;
	}
	else
	{
		g_sfn->next = calloc(1, sizeof(*g_sfn));
		if (g_sfn->next == nullptr)
		{
			fprintf(stderr, "sohomot: failed to register test\n");
			return;
		}

		g_sfn = g_sfn->next;
		g_sfn->func = func;
		g_sfn->suite_name = suitename;
		g_sfn->test_name = testname;
		g_sfn->next = nullptr;
		++g_node_len;
	}
}

bool init_arr_func_nodes(void)
{
	g_sfn = g_sfn_head;
	arr_func_nodes = calloc(g_node_len, sizeof(*g_sfn));
	if (arr_func_nodes == nullptr)
		return false;
	errno = 0;

	uint32_t i = 0;
	while (g_sfn != nullptr)
	{
		arr_func_nodes[i] = *g_sfn;
		g_sfn = g_sfn->next;
		++i;
	}

	struct sohomot_func_node* temp = nullptr;
	g_sfn = g_sfn_head;
	while (g_sfn != nullptr)
	{
		temp = g_sfn->next;
		free(g_sfn);
		g_sfn = temp;
	}
	return true;
}

void free_arr_func_nodes(void)
{
	free(arr_func_nodes);
	arr_func_nodes = nullptr;
}

struct sohomot_func_node* get_nodes(void)
{
	return arr_func_nodes;
}

uint32_t get_total_nodes(void)
{
	return g_node_len;
}

bool do_nodes_match(struct sohomot_func_node* n1, \
		struct sohomot_func_node* n2)
{
	if (n1->func != n2->func)
		return false;
	if (strcmp(n1->suite_name, n2->suite_name) != 0)
		return false;
	if (strcmp(n1->test_name, n2->test_name) != 0)
		return false;
	return true;
}
