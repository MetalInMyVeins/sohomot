#include "sohomot/sohomot.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

void print_int8(const char* type, const char* name, \
		const int8_t x, const char* side)
{
	printf("    %s (%s) %s -> '%c' (%d) (0x%x)\n", \
			side, type, name, x, (int32_t)x, (int32_t)x);
}

void print_uint8(const char* type, const char* name, \
		const uint8_t x, const char* side)
{
	printf("    %s (%s) %s -> '%c' (%u) (0x%x)\n", \
			side, type, name, x, (uint32_t)x, (uint32_t)x);
}

void print_int16(const char* type, const char* name, \
		const int16_t x, const char* side)
{
	printf("    %s (%s) %s -> %hd (0x%x)\n", \
			side, type, name, x, (int32_t)x);
}

void print_uint16(const char* type, const char* name, \
		const uint16_t x, const char* side)
{
	printf("    %s (%s) %s -> %hu (0x%x)\n", \
			side, type, name, x, (uint32_t)x);
}

void print_int32(const char* type, const char* name, \
		const int32_t x, const char* side)
{
	printf("    %s (%s) %s -> %d (0x%x)\n", \
			side, type, name, x, (int32_t)x);
}

void print_uint32(const char* type, const char* name, \
		const uint32_t x, const char* side)
{
	printf("    %s (%s) %s -> %u (0x%x)\n", \
			side, type, name, x, (int32_t)x);
}

void print_int64(const char* type, const char* name, \
		const int64_t x, const char* side)
{
	printf("    %s (%s) %s -> %" PRId64 " (0x%" PRIx64 ")\n", \
			side, type, name, x, x);
}

void print_uint64(const char* type, const char* name, \
		const uint64_t x, const char* side)
{
	printf("    %s (%s) %s -> %" PRIu64 " (0x%" PRIx64 ")\n", \
			side, type, name, x, x);
}

void print_long_long(const char* type, const char* name, \
		const long long x, const char* side)
{
	printf("    %s (%s) %s -> %" PRId64 " (0x%" PRIx64 ")\n", \
			side, type, name, (int64_t)x, (uint64_t)x);
}

void print_unsigned_long_long(const char* type, const char* name, \
		const unsigned long long x, const char* side)
{
	printf("    %s (%s) %s -> %" PRIu64 " (0x%" PRIx64 ")\n", \
			side, type, name, (uint64_t)x, (uint64_t)x);
}

void print_double(const char* type, const char* name, \
		const double x, const char* side)
{
	printf("    %s (%s) %s -> %.18lf\n", \
			side, type, name, x);
}

void print_long_double(const char* type, const char* name, \
		const long double x, const char* side)
{
	printf("    %s (%s) %s -> %.18Lf\n", \
			side, type, name, x);
}

void print_float(const char* type, const char* name, \
		const float x, const char* side)
{
	printf("    %s (%s) %s -> %.6f\n", \
			side, type, name, (double)x);
}

void print_str(const char* type, const char* name, \
		const char* x, const char* side)
{
	if (x != nullptr)
		printf("    %s (%s) %s -> \"%s\"\n", \
				side, type, name, x);
	else
		printf("    %s (%s) %s -> %s\n", \
				side, type, name, "nullptr");
}

void print_bool(const char* type, const char* name, \
		bool x, const char* side)
{
	printf("    %s (%s) %s -> %s\n", \
			side, type, name, x? "true" : "false");
}

void print_void_ptr(const char* type, const char* name, \
		const void* x, const char* side)
{
	if (x == nullptr)
		printf("    %s (%s) %s -> %s\n", \
				side, type, name, "nullptr");
	else
		printf("    %s (%s) %s -> %p\n", \
				side, type, name, x);
}

void print_default(void)
{
	printf("Unknown type\n");
}
