#ifndef SOHOMOT_H
#define SOHOMOT_H

#include <stdint.h>
#include <stdio.h>

struct sohomot_func_node;
typedef void (*sohomot_func)(void);
void register_sohomot_func(sohomot_func, const char* suitename, \
		const char* testname);
bool init_arr_func_nodes(void);
void free_arr_func_nodes(void);
struct sohomot_func_node* get_nodes(void);
uint32_t get_total_nodes(void);
bool do_nodes_match(struct sohomot_func_node* n1, struct \
		sohomot_func_node* n2);
void set_expect_passed(bool passed);
bool expect_streq(const char* a, const char* b);

static const char* str_equality = "Expected equality of these values:";
static const char* str_expectation_not_met = "Expectation not met for these values:";
static const char* str_lhs = "LHS:";
static const char* str_rhs = "RHS:";

#define TEST(Suite, Test) \
	void sohomot_##Suite##_##Test(void); \
	__attribute__((constructor)) \
	static void register_##Suite##_##Test(void) \
	{ \
		register_sohomot_func(sohomot_##Suite##_##Test, #Suite, #Test); \
	} \
	void sohomot_##Suite##_##Test(void)

void print_int8(const char* type, const char* name, \
		const int8_t x, const char* side);
void print_uint8(const char* type, const char* name, \
		const uint8_t x, const char* side);
void print_int16(const char* type, const char* name, \
		const int16_t x, const char* side);
void print_uint16(const char* type, const char* name,
		const uint16_t x, const char* side);
void print_int32(const char* type, const char* name, \
		const int32_t x, const char* side);
void print_uint32(const char* type, const char* name, \
		const uint32_t x, const char* side);
void print_int64(const char* type, const char* name, \
		const int64_t x, const char* side);
void print_uint64(const char* type, const char* name, \
		const uint64_t x, const char* side);
void print_long_long(const char* type, const char* name, \
		const long long x, const char* side);
void print_unsigned_long_long(const char* type, const char* name, \
		const unsigned long long x, const char* side);
void print_str(const char* type, const char* name, \
		const char* x, const char* side);
void print_default(void);

#define TYPESTR(X) _Generic((X), \
	char: "char", \
	int8_t: "int8_t", \
	uint8_t: "uint8_t", \
	int16_t: "int16_t", \
	uint16_t: "uint16_t", \
	int32_t: "int32_t", \
	uint32_t: "uint32_t", \
	int64_t: "int64_t", \
	uint64_t: "uint64_t", \
	long long: "long long", \
	unsigned long long: "unsigned long long", \
	char*: "char*", \
	const char*: "const char*", \
	default: "unknown" \
)

#define PRINT(T, Y, X, Z) _Generic((X), \
	char: print_int8, \
	int8_t: print_int8, \
	uint8_t: print_uint8, \
	int16_t: print_int16, \
	uint16_t: print_uint16, \
	int32_t: print_int32, \
	uint32_t: print_uint32, \
	int64_t: print_int64, \
	uint64_t: print_uint64, \
	long long: print_long_long, \
	unsigned long long: print_unsigned_long_long, \
	char*: print_str, \
	const char*: print_str, \
	default: print_default \
)(T, Y, X, Z)

#define EXPECT_EQ(A, B) \
	do \
	{ \
		typeof(A) _a = (A); \
		typeof(B) _b = (B); \
		if (!(_a == _b)) \
		{ \
			printf("%s:%d: Failure\n", __FILE__, __LINE__); \
			printf("%s\n", str_equality); \
			PRINT(TYPESTR(A), #A, A, str_lhs); \
			PRINT(TYPESTR(B), #B, B, str_rhs); \
			printf("\n"); \
			set_expect_passed(false); \
		} \
	} while (false)

#define EXPECT_STREQ(A, B) \
	do \
	{ \
		const char* _a = (A); \
		const char* _b = (B); \
		if (!expect_streq(_a, _b)) \
		{ \
			printf("%s:%d: Failure\n", __FILE__, __LINE__); \
			printf("%s\n", str_equality); \
			PRINT(TYPESTR(A), #A, A, str_lhs); \
			PRINT(TYPESTR(B), #B, B, str_rhs); \
			printf("\n"); \
			set_expect_passed(false); \
		} \
	} while (false)

#define EXPECT_NE(A, B) \
	do \
	{ \
		typeof(A) _a = (A); \
		typeof(B) _b = (B); \
		if (_a == _b) \
		{ \
			printf("%s:%d: Failure\n", __FILE__, __LINE__); \
			printf("%s\n", str_expectation_not_met); \
			printf("    Should be: LHS != RHS\n"); \
			PRINT(TYPESTR(A), #A, A, str_lhs); \
			PRINT(TYPESTR(B), #B, B, str_rhs); \
			printf("\n"); \
			set_expect_passed(false); \
		} \
	} while (false)

#define EXPECT_LT(A, B) \
	do \
	{ \
		typeof(A) _a = (A); \
		typeof(B) _b = (B); \
		if (_a >= _b) \
		{ \
			printf("%s:%d: Failure\n", __FILE__, __LINE__); \
			printf("%s\n", str_expectation_not_met); \
			printf("    Should be: LHS < RHS\n"); \
			PRINT(TYPESTR(A), #A, A, str_lhs); \
			PRINT(TYPESTR(B), #B, B, str_rhs); \
			printf("\n"); \
			set_expect_passed(false); \
		} \
	} while (false)

#define EXPECT_LE(A, B) \
	do \
	{ \
		typeof(A) _a = (A); \
		typeof(B) _b = (B); \
		if (_a > _b) \
		{ \
			printf("%s:%d: Failure\n", __FILE__, __LINE__); \
			printf("%s\n", str_expectation_not_met); \
			printf("    Should be: LHS <= RHS\n"); \
			PRINT(TYPESTR(A), #A, A, str_lhs); \
			PRINT(TYPESTR(B), #B, B, str_rhs); \
			printf("\n"); \
			set_expect_passed(false); \
		} \
	} while (false)

#endif // SOHOMOT_H
