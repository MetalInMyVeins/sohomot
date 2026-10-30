<h1 align="center">sohomot</h1>

<p align="center">
  <img src="./res/sohomot_bangla.png" />
</p>

Drop-in minimal replacement of gtest written in C23 given that you only use:

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

I always heavily relied on gtest while working with C++. But for being written in C++, using gtest in a plain C project meant bringing the C++ runtime which seemed quite awkward to me. At the same time, I didn't want to loose the comfort gtest provided. That's the sole purpose of this project. Not having to deal with C++ while having access to almost the same facility gtest provided.

"Sohomot" is a bangla word which literally means "agreement" or "in agreement with" in a joking tone.

# Dependency:

- A C23 compliant compiler
- Any linker
- Linux (other than C23, it makes use of exactly one compiler extension and one POSIX function, so it shouldn't be portable out of the box)

# Usage

Clone the repo and build:

```bash
git clone https://github.com/MetalInMyVeins/sohomot
cd sohomot
# You can execute `make info` or only `make` to list all available targets.
make rel
```

This would build the library in `build/rel/lib/`. The target would be `libsohomot_main.so` which would be symlinked to `libsohomot_main.so.0.1.0`.

The main header file would be located at `include/sohomot/sohomot.h`.

To use the library, write a C program with no `main()`:

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

Compile and run:
```bash
clang -std=c23 test.c -o test \
        -Iinclude \
        -Lbuild/rel/lib \
        -lsohomot_main \
        -Wl,-rpath=build/rel/lib

./test
```

The output quality is arguably better than gtest:
```diff
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
Execution time of each test is measured in nanoseconds, cast to microseconds, and then reported.

# Performance

19 test suites containing 55 tests in total were benchmarked against both `sohomot` and `gtest`. Unsurprisingly, `sohomot` outperforms `gtest` by `2.5x` in execution speed while using `30%` less memory. As `sohomot` is still new, there are lots of places where I took bad decisions. I believe optimizing them would improve performance even more.

# LLM Ban

Except for the AI-generated image, this repo is entirely human-written.

LLMs are strictly banned from contributing.

# License

MIT
