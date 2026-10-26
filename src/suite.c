#include "suite.h"
#include "sohomot/sohomot.h"
#include <errno.h>
#include <stdlib.h>

static constexpr uint32_t PACKET_SIZE = 256;
static struct test_suite* g_suites = nullptr;

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
