#include "sohomot/sohomot.h"
#include "suite.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	setvbuf(stdout, nullptr, _IONBF, 0);
	setvbuf(stderr, nullptr, _IONBF, 0);
	if (init_arr_func_nodes() == false)
	{
		fprintf(stderr, "sohomot: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if (suites_init() == false)
	{
		fprintf(stderr, "sohomot: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	bool suites_setup = setup_suites(get_nodes(), get_total_nodes());
	if (suites_setup == false)
	{
		fprintf(stderr, "sohomot: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	print_suites_status();
	clean_suites();
	suites_free();
	free_arr_func_nodes();
	return EXIT_SUCCESS;
}
