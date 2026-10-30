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

The purpose of this post is to explain the implementation details and design decisions of `sohomot`.

<p align="center">
  <img src="./assets/sohomot_bangla.png" />
</p>
<h5 align="center"><i>sohomot is a bangla word and this literally reads as someone is telling you, "i agree bro, BLEGH"</i></h5>
