---
layout: blog
title: "sohomot"
description: "Minimal drop-in replacement of gtest written in C23"
date: 2026-10-30
permalink: /
---

## Intro

Why write another test framework when there's gtest, right? gtest provides so many features that you might start using it and never look back again. From basic tests, test fixtures and state management to data-driven and parameterized testing, death tests, exception isolation; gtest provides it all. If you routinely use C++, gtest might be an integral part of your setup. But it's all fun and games until you work on a plain C project. Suddenly everything changes. You CANNOT include and use gtest in a plain C program. A C compiler is incapable of parsing gtest headers as they're entirely written in C++. So you'd need to write the tests in a C++ file and compile using a C++ compiler. And there comes the C++ runtime in your test binary. And in no time, you'd find yourself managing a toolchain just for C++ and adding `extern "C"` guards to your headers to disable name mangling. Imagine writing plain C and still having to deal with C++ for tests. Yes there are other test frameworks available for plain C. BUT YOU DON'T GET GTEST!

That's when I realized I need a C equivalent of gtest. As in, the said library should be capable of being used exactly the same way gtest is used and should report errors the same way (if not better) gtest does. That's where `sohomot` comes. `sohomot` is written from scratch, by hand, in C23, mimicking gtest. Other than C23, `sohomot` also makes use of a compiler extension and a POSIX function. So currently it works only in x86_64 linux.

The purpose of this post is to explain the implementation details and design decisions of `sohomot`. I'd arrange it mimicking the exact sequence the development took place.

<p align="center">
  <img src="./assets/sohomot_bangla.png" />
</p>
<h5 align="center"><i>sohomot is a bangla word; the image literally reads as someone is telling you, "i agree bro, BLEGH"</i></h5>

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

If you had split that compilation step, you'd see that the compiler first translated the source files to object files, and then invoked a linker on those files which finally created the binary `main`. Given that you execute `main` in linux, the kernel first loads the executable, the dynamic linker loads the shared libraries and performs relocations, and then control passes to `_start` which was provided to the program by `crt1.o` during linking step. `_start` calls `__libc_start_main()` provided by glibc and `__libc_start_main()` eventually calls `main()` of your program and that's when your actual instructions start executing. Too simple, right? If you defined multiple `main()` in a single translation unit, it would violate ODR and wouldn't compile. If you tried to link multiple `main()` from different translation units, you'd get a multiple definition error. So, if you're not performing any compiler trickery, there can be exactly one `main()`. That's it.

Now, back to your `add` function. You want to test it using gtest. You write a program.

`test.cxx`:

```cpp
#include "add.hxx"
#include <gtest/gtest.h>

TEST(Integers, Add)
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

Now let's face the problem. You don't really know how gtest is actually getting hands on that `TEST` macro and its parameters. I mean, whatever `main()` you write, the point when `main()` starts executing, you'd either have to be able to parse the `TEST`s somehow, or already have access to the parsed `TEST`s or whatever they are. You look up how gtest does it. Find out it's static initialization. Good. Then look up how to do it in C and you're cooked. Because in C, variables with static storage duration MUST be initialized with a compile-time constant expression. There's nothing called "runtime dynamic initialization" in standard C. If that's not possible, how can you even auto-register the `TEST`s? And not only that, even if it was possible, you'd still have to think of a way to define those `TEST`s in a way so they can be stored.

## TA-DA!

Digging through possible solutions, somethings suddenly catches your eye. ELF binaries have a special section called `.init_array`. This section holds an array of function pointers. When a binary is loaded into memory and execution begins, the C runtime steps through `.init_array` and calls each function pointer stored in it. And that happens before `main()` even starts. So what if there was a way to make each `TEST` a function in disguise and auto-register them in `.init_array`? Exactly! That's the exact point development of `sohomot` began.
