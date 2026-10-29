# sohomot

<table>
  <tr>
    <td align="center">
      <img src="./res/sohomot_bangla.png" width="400" />
      <br />
    </td>
    <td align="center">
      <img src="./res/sohomot_english.png" width="400" />
      <br />
    </td>
  </tr>
</table>

Drop-in minimal replacement of gtest written in C23 given that you only use:

- `TEST(TestSuite, TestCase)`
- `EXPECT_EQ(EXPR1, EXPR2)`
- `EXPECT_STREQ(EXPR1, EXPR2)`

I always heavily relied on gtest while working with C++. But as gtest is written in C++, using it in a plain C project means bringing the C++ runtime which seemed kind of awkward to me. At the same time, I didn't want to loose the comfort gtest provided. That's the sole purpose of this project.

"Sohomot" is a bangla word which literally means "agreement" or "in agreement with". It's also used in mocking blind flatterers, sycophants, and yes-mans.

# LLM Ban

This is an educational project. Except for the AI-generated images, this repo is entirely human-written.

LLMs are strictly banned from contributing.

# License

MIT
