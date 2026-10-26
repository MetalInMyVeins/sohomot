#include "sohomot/sohomot.h"
#include "suite.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	if (init_arr_func_nodes() == false)
	{
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	suites_init();
	suites_free();
	free_arr_func_nodes();
	return EXIT_SUCCESS;
}
