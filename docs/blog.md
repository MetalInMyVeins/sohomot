---
layout: blog
title: "sohomot"
description: "Minimal drop-in replacement of gtest written in C23"
date: 2026-10-30
permalink: /
---

## Intro

Why write another test framework when there's gtest, right? gtest provides so many features that you might start using them and never look back again. From basic tests, test fixtures and state management to data-driven and parameterized testing, death tests, exception isolation; gtest provides it all. If you routinely use C++, gtest might be an integral part of your setup. But it's all fun and games until you work on a plain C project. Suddenly everything changes. You CANNOT include and use gtest in a plain C program! A C compiler is incapable of parsing gtest headers as they're entirely written in C++. So you'd need to write the tests in a C++ file and compile using a C++ compiler. And there comes the C++ runtime in your test binary. In no time, you'd find yourself managing a toolchain just for C++ and adding `extern "C"` guards to your headers to disable name mangling. It's unquestionably annoying! Imagine writing plain C and still having to deal with C++ for tests. Yes there are other test frameworks available for plain C. BUT YOU DON'T GET GTEST!

That's when I realized I needed a C equivalent of gtest. As in, the said library should be capable of being used exactly the same way gtest is used and should report errors the same way (if not better) gtest does. That's where `sohomot` comes. `sohomot` is written from scratch, by hand, in C23, mimicking gtest. Other than C23, `sohomot` also makes use of a compiler extension and a POSIX function. So currently it works only in x86_64 linux.

And note that, `sohomot` currently supports only:

- `TEST(TestSuite, TestCase)`
- `EXPECT_EQ(EXPR1, EXPR2)`
- `EXPECT_NE(EXPR1, EXPR2)`
- `EXPECT_LT(EXPR1, EXPR2)`
- `EXPECT_LE(EXPR1, EXPR2)`
- `EXPECT_GT(EXPR1, EXPR2)`
- `EXPECT_GE(EXPR1, EXPR2)`
- `EXPECT_DOUBLE_EQ(EXPR1, EXPR2)`
- `EXPECT_LONG_DOUBLE_EQ(EXPR1, EXPR2)`   (extension)
- `EXPECT_FLOAT_EQ(EXPR1, EXPR2)`
- `EXPECT_STREQ(EXPR1, EXPR2)`
- `EXPECT_STRNE(EXPR1, EXPR2)`
- `EXPECT_STRCASEEQ(EXPR1, EXPR2)`
- `EXPECT_STRCASENE(EXPR1, EXPR2)`
- `EXPECT_TRUE(EXPR)`
- `EXPECT_FALSE(EXPR)`

There's no `ASSERT_*` as of now. I'd add everything incrementally as I need them.

The purpose of this post is to explain the implementation details and design decisions of `sohomot`. I'd arrange it mimicking the exact sequence the development took place.

<p align="center">
  <img src="./assets/sohomot_bangla.png" />
</p>
<h5 align="center"><i>sohomot is a bangla word; the image literally reads as if someone is telling you, "i agree bro, BLEGH"</i></h5>

## Problem Scoping

Let's first see how gtest works. Say, you write a function `add` that adds two integers and returns the addition. You decide to split the header and implementation in two files `add.hxx` and `add.cxx`.

`add.hxx`:
```cpp
int add(int, int);
```

`add.cxx`:
```cpp
int add(int x, int y)
{
    return x + y;
}
```

And then to use the function, you write a program.

`main.cxx`:
```cpp
#include "add.hxx"

int main()
{
    int z = add(3, 5);
    return z;
}
```

You compile it like this:
```bash
clang++ -std=c++20 main.cxx add.cxx -o main
```

If you had split that compilation step, you'd see that the compiler first translated the source files to object files, and then invoked a linker on those files which finally created the binary `main`. Given that you execute `main` in linux, the kernel first loads the executable, the dynamic linker loads the shared libraries and performs relocations, and then control passes to `_start` which was provided to the program by `crt1.o` during linking step. `_start` calls `__libc_start_main()` provided by glibc and `__libc_start_main()` eventually calls `main()` of your program and that's when your actual instructions start executing. Too simple, right? If you defined multiple `main()` in a single translation unit, that would be straight up a redefinition error and wouldn't compile. If you tried to link multiple `main()` from different translation units, that'd be ODR violation. So, if you're not performing any compiler trickery, there can be exactly one `main()`. That's it.

Now, back to your `add` function. You want to test it using gtest. You write a program.

`test.cxx`:
```cpp
#include "add.hxx"
#include <gtest/gtest.h>

TEST(Integer, Add)
{
    EXPECT_EQ(add(3, 5), 8);
    EXPECT_EQ(add(2, 9), 11);
    EXPECT_EQ(add(3, -5), -2);
    EXPECT_EQ(add(13, 15), 28);
    EXPECT_NE(add(33, 55), 8);
}
```

That's it, that's the test. Notice there's no `main()` in the file because we'd compile it like this:

```bash
clang++ -std=c++20 test.cxx add.cxx -lgtest -lgtest_main -o test
```

`gtest_main` provides the `main()` to `test` which contains the machinery to initialize and run the tests. Running `test` gets this:
```
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from Integers
[ RUN      ] Integers.Add
[       OK ] Integers.Add (0 ms)
[----------] 1 test from Integers (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

You can try providing an empty `main()` to your test file and that would override `gtest_main` (if you link it) and you'd see no output.

## Facing the Problem

Now let's face the problem. You don't really know how gtest is actually getting its hands on that `TEST` macro and its parameters. I mean, whatever `main()` you write, the point when `main()` starts executing, you'd either have to be able to parse the `TEST`s somehow, or already have access to the parsed `TEST`s or whatever they are. You look up how gtest does it. Find out they do it through dynamic initialization of namespace scope static objects. Good. Then look up how to do it in C and you're cooked. Because in C, variables with static storage duration MUST be initialized with a compile-time constant expression. There's nothing called "runtime dynamic initialization" in standard C. If that's not possible, how can you even auto-register the `TEST`s? And not only that, even if it was possible, you'd still have to think of a way to define those `TEST`s in a way so they can be stored.

## TA-DA!

Digging through possible solutions, somethings suddenly catches your eye. ELF binaries have a special section called `.init_array`. This section holds an array of function pointers. When a binary is loaded into memory and execution begins, the C runtime steps through `.init_array` and calls each function pointer stored in it. And that happens before `main()` even starts. So what if there was a way to make each `TEST` a function in disguise and auto-register them in `.init_array`? Exactly! That's the exact point development of `sohomot` began.

## Implementation

### The Hard Part

Remember the gtest output? It comprehensively reports the passed and failed tests in detail. Put the necessary points in a list:

- `x` tests from `y` test suites ran
- test `foo` of test suite `bar` passed/failed
- `p` tests passed while `q` tests failed
- test `foo.bar` took `t` seconds to run

So starting from the most fundamental point, if we provide a `TEST(Suite, Test)` macro, we have to catch whatever string is in `Suite` and `Test`, store them, make unique functions out of them, and store the function pointers. This can be done using a macro trick. You can token paste the macro parameters and attach them together like I did:
```c
#define TEST(Suite, Test) \
    void sohomot_##Suite##_##Test(void);
```

That means if you defined `TEST(String, Memcpy)`, it would nicely forward declare a function `void sohomot_String_Memcpy(void)` instead. By the time the program would start, the functions would already get allocated valid addresses. So you can easily store the function pointer `sohomot_String_Memcpy` in a variable of type `void (*)(void)`.

Now about the storing part. You need to store 3 things:

- the function pointer: `void (*)(void)`
- the test suite name: `const char*`
- the test name: `const char*`

You can build a struct out of these and create an array of that struct. But do you know how many functions you might have? No. You don't know how many tests are going to be there so you can't allocate the required amount of space. So a linked list is your last hope. In whatever way you store the test cases, you need to be able to automatically attach them together.

That's how `sohomot_func_node` is born:
```c
typedef void (*sohomot_func)(void);

struct sohomot_func_node
{
	sohomot_func func;
	const char* suite_name;
	const char* test_name;
	struct sohomot_func_node* next;
};
```

The next part is easy, write a registration function `register_sohomot_func(sohomot_func, const char*, const char*)` which registers the function pointer along with stringified test suite and test name.

But the fundamental problem is not yet solved. You wrote the registration function. But WHO calls it? Right? The linked list cannot be built if no one even calls your registration function. Apparently, your registration function should be called by someone, aka, a function that already resides in `.init_array`. How do you send someone to `.init_array`? Right, that's where the compiler extension comes.

`__attribute__((constructor))` is a gcc/clang function attribute that registers the said function to `.init_array`. Say you write:
```c
__attribute__((constructor)) static void foo(void)
{
    do_bla();
}
```
`foo` would be registered in `.init_array` and `do_bla` would be called before anything in `main()` starts. That's the trick. You have to register a unique function to `.init_array` using `__attribute__((constructor))` and that unique function should call your registration function in it. That's how by the time `main()` starts, you'd already have the linked list that you wanted.

But wait, `TEST` has function like body, how to store that? Well that's another easy trick. Remember the forward declaration `void sohomot_##Suite##_##Test(void);`? Well, define it and end the macro! Pretend as if it's the start of your test function.

So the entire `TEST` macro can be written as:
```c
#define TEST(Suite, Test) \
	void sohomot_##Suite##_##Test(void); \
	__attribute__((constructor)) \
	static void register_##Suite##_##Test(void) \
	{ \
		register_sohomot_func(sohomot_##Suite##_##Test, #Suite, #Test); \
	} \
	void sohomot_##Suite##_##Test(void)
```

And the test in `test.cxx` would be replaced with:
```c
void sohomot_Integer_Add(void);
__attribute__((constructor))
static void register_Integer_Add(void)
{
    register_sohomot_func(sohomot_Integer_Add, "Integer", "Add");
}
void sohomot_Integer_Add(void)
{
    EXPECT_EQ(add(3, 5), 8);
    EXPECT_EQ(add(2, 9), 11);
    EXPECT_EQ(add(3, -5), -2);
    EXPECT_EQ(add(13, 15), 28);
    EXPECT_NE(add(33, 55), 8);
}
```

Cool right? Yeah.

### The Not-So-Hard Part

Now that you got your hands on linked list containing structs containing all the test functions, the next part is easy, although not small by any means. You'd be able to notice that a test case is part of a test suite. You'd run `x` number of test cases from `y` number of test suites and report a pretty output. For that, you'd need to differentiate each test case by their parent test suite.

For that, my idea was really simple. Build a struct `test_suite` which would have its own name and would hold a dynamic array of its children test cases, aka, `sohomot_func_node`s which have the same parent suite name as the `test_suite`'s name. And finally build a dynamic AoS of `test_suite`s. Later when you'd walk the linked list of test cases, you'd dynamically populate the array of `test_suite`s and store `sohomot_func_node` children in them as required (currently there's a minor bottleneck in the procedure which I'd solve later).

This is the so called `test_suite`:
```c
struct test_suite
{
	const char* suite_name;
	struct sohomot_func_node* nodes_holder;
	uint32_t holder_cap;
	uint32_t holder_last_index;
};
```

And later, you'd loop on the AoS of `test_suite` and call each function held by each `test_suite`. The `EXPECT_*` macros would change the state of the test case and test suite during runtime as required and finally success or error status would be reported which is, in my opinion, prettier and more comprehensive than gtest.

The time interval between the start and finish of a test case is measured in nanosecond accuracy for which a POSIX function `clock_gettime()` is used. The result is cast down do microseconds and then reported.

## Demo

If you followed the project's ![README.md](https://github.com/MetalInMyVeins/sohomot/blob/main/README.md) and properly built the library, the following test file would result in the output given after that.

`test.c`:
```c
#include "sohomot/sohomot.h"
#include <stdint.h>

TEST(Integral, Add)
{
	int x = 111;
	int y = 222;
	EXPECT_NE(x, y);
	x += 111;
	EXPECT_LT(x, y);
	y += 333;
	EXPECT_NE(x, y);
}

TEST(Integral, Sub)
{
	uint64_t abcd = 12345;
	uint64_t efgh = 54321;
	abcd += 41976;
	EXPECT_EQ(abcd, efgh);
}

TEST(String, Equality)
{
	const char* s1 = "asdf";
	const char* s2 = "qwer";
	EXPECT_STREQ(s1, s2);
	s2 = "asdf";
	EXPECT_STREQ(s1, s2);
	s1 = "asdg";
	EXPECT_STREQ(s1, s2);
}
```

Output:
```
[==========] Running 3 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 2 tests from Integral
[ RUN      ] Integral.Add
scratch/t3.c:10: Failure
Expectation not met for these values:
    Should be: LHS < RHS
    LHS: (int32_t) x -> 222 (0xde)
    RHS: (int32_t) y -> 222 (0xde)

[  FAILED  ] Integral.Add   (35.38 us)
[ RUN      ] Integral.Sub
[       OK ] Integral.Sub   (0.15 us)
[----------] 2 tests from Integral   (35.54 us total)

[----------] 1 test from String
[ RUN      ] String.Equality
scratch/t3.c:27: Failure
Expected equality of these values:
    LHS: (const char*) s1 -> "asdf"
    RHS: (const char*) s2 -> "qwer"

scratch/t3.c:31: Failure
Expected equality of these values:
    LHS: (const char*) s1 -> "asdg"
    RHS: (const char*) s2 -> "asdf"

[  FAILED  ] String.Equality   (86.45 us)
[----------] 1 test from String   (86.45 us total)

[----------] Global test environment tear-down
[==========] 3 test from 2 test suites ran.   (121.99 us total)
[  PASSED  ] 1 test.
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] Integral.Add
[  FAILED  ] String.Equality

2 FAILED TESTS
```

## Conclusion

There are still lots of work and optimization to do. The next thing I'd add are the `ASSERT_*` macros. Anyways, was a fun project.
