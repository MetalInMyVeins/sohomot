<h1 align="center">sohomot</h1>

<p align="center">
  <img src="./res/sohomot_bangla.png" />
</p>

Drop-in minimal replacement of gtest written in C23 given that you only use:

- `TEST(TestSuite, TestCase)`
- `EXPECT_EQ(EXPR1, EXPR2)`
- `EXPECT_STREQ(EXPR1, EXPR2)`
- `EXPECT_NE(EXPR1, EXPR2)`
- `EXPECT_LT(EXPR1, EXPR2)`
- `EXPECT_LE(EXPR1, EXPR2)`
- `EXPECT_GT(EXPR1, EXPR2)`
- `EXPECT_GE(EXPR1, EXPR2)`

I always heavily relied on gtest while working with C++. But as gtest is written in C++, using it in a plain C project meant bringing the C++ runtime which seemed kind of awkward to me. At the same time, I didn't want to loose the comfort gtest provided. That's the sole purpose of this project.

"Sohomot" is a bangla word which literally means "agreement" or "in agreement with" in a joking tone.

# LLM Ban

This is an educational project. Except for the AI-generated images, this repo is entirely human-written.

LLMs are strictly banned from contributing.

# License

MIT
