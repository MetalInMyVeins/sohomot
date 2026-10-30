---
layout: blog
title: "sohomot"
description: "Minimal drop-in replacement of gtest written in C23"
date: 2026-10-30
permalink: /
---

Why write another test framework when there's gtest, right? gtest provides so many features that you might start using it and never look back again. From basic tests, test fixtures and state management to data-driven and parameterized testing, death tests, exception isolation; gtest provides it all. If you routinely use C++, gtest might be an integral part of your setup. But it's all fun and games until you work on a plain C project. Suddenly everything changes. You CANNOT include and use gtest in a plain C program. A C compiler is incapable of parsing gtest headers as they're entirely written in C++. So you'd need to write the tests in a C++ file and compile using a C++ compiler. And there comes the C++ runtime in your test binary. And in no time, you'd find yourself managing a toolchain just for C++ and adding `extern "C"` guards to your headers to disable name mangling. Imagine writing plain C and still having to deal with C++ for tests. Yes there are other test frameworks available for plain C. BUT YOU DON'T GET GTEST!
