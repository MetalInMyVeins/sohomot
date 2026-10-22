#include "sohomot/sohomot.h"
#include "suite.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	if (suites_init() == false)
	{
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	suites_free();
	return EXIT_SUCCESS;
}
