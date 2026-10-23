#include "sohomot/sohomot.h"
#include <stdlib.h>

struct sohomot_func_node
{
	sohomot_func func;
	struct sohomot_func_node* next;
};

static struct sohomot_func_node* g_sfn = nullptr;
static struct sohomot_func_node* g_sfn_head = nullptr;

void register_sohomot_func(sohomot_func func)
{
	if (g_sfn == nullptr)
	{
		g_sfn = malloc(sizeof(*g_sfn));
		g_sfn->func = func;
		g_sfn->next = nullptr;
		g_sfn_head = g_sfn;
	}
	else
	{
		g_sfn->next = malloc(sizeof(*g_sfn));
		g_sfn = g_sfn->next;
		g_sfn->next = nullptr;
	}
}

void free_sohomot_funcs(void)
{
	g_sfn = g_sfn_head;

	while (g_sfn != nullptr)
	{
		free(g_sfn);
		g_sfn = g_sfn->next;
	}
}
