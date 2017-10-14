/* Simple C++ testing library: example of failing tests.
 * Copyright: 2014-2017, Dmitry Shachnev.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "testing.hpp"

struct FailingTestCase: TestCase {
  void run() override {
    ASSERT_TRUE(false);
    ASSERT_FALSE(true);
    ASSERT_EQUAL(1 + 1, 3);
    ASSERT_ALMOST_EQUAL(1, 1.5, 0.4);
    ASSERT_STRINGS_EQUAL("foo", std::string("bar"));
    std::vector<int> v = { 0, 1 };
    ASSERT_THROWS(std::out_of_range, v.at(1));
    void (*func)() = nullptr;
    func();
    ASSERT_TRUE(true);
  }
};

REGISTER_TEST(FailingTestCase, "Failing tests");
