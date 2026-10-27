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

This is a very minimal gtest-like test framework written in C. With my past reliance on gtest, it was hard to get rid of it. At the same time, it was awkward to bring the C++ runtime in a C project just for testing. And also, I didn't wan't to loose the comfort gtest provided. So this would work exactly the way gtest works given that you only use:

- `TEST(TestSuite, TestCase)`
- `EXPECT_EQ(EXPR1, EXPR2)`
- `EXPECT_STREQ(EXPR1, EXPR2)`

"Sohomot" is a bangla word which literally means "agreement" or "in agreement with". It's also used in mocking blind flatterers, sycophants, and yes-mans.

# LLM Ban

This is an educational project. LLMs are strictly banned from contributing.
The images are AI-generated.

# License

MIT
