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
	}
}

void free_sohomot_funcs(void)
{
	struct sohomot_func_node* temp = g_sfn_head;
	while (temp != nullptr)
	{
		temp = g_sfn_head->next;
		free(g_sfn_head);
		g_sfn_head = temp;
	}
}

struct sohomot_func_node* get_nodes(void)
{
	return g_sfn_head;
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
