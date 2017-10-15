/* Simple C++ testing library.
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

#ifndef CXXUNIT_TESTING_HPP
#define CXXUNIT_TESTING_HPP

#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <exception>
#include <iostream>
#include <vector>
#include "printing.hpp"
#ifdef __linux__
#  include <csignal>
#  include <execinfo.h>
#endif

#ifdef _MSC_VER
#  define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define REGISTER_TEST(cls, name) \
  static TestCaseProcessor _##cls({new cls(), name})

#define PRINT_ERROR() \
  std::cerr << "   " << E_ERROR("ERROR") << " in `" << __PRETTY_FUNCTION__ << "' at " << __FILE__ \
            << " line " << __LINE__ << ":" << std::endl;

#define ASSERT_TRUE(expression) \
  do { \
    auto _value = (expression); \
    if (!do_assert(_value)) { \
      PRINT_ERROR(); \
      std::cerr << "     Expression `" #expression "' is not true." << std::endl; \
      std::cerr << "     Value is " << repr(_value) << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_FALSE(expression) \
  do { \
    auto _value = (expression); \
    if (!do_assert(!_value)) { \
      PRINT_ERROR(); \
      std::cerr << "     Expression `" #expression "' is not false." << std::endl; \
      std::cerr << "     Value is " << repr(_value) << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_RELATION(e1, rel, e2) \
  do { \
    auto _v1 = (e1); \
    auto _v2 = (e2); \
    if (!do_assert(_v1 rel _v2)) { \
      PRINT_ERROR(); \
      std::cerr << "     Expression `" #e1 " " #rel " " #e2 "' is not true." << std::endl; \
      std::cerr << "     " << #e1 " = " << repr(_v1) << ", " #e2 " = " << repr(_v2) << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_EQUAL(e1, e2) \
  do { \
    auto _v1 = (e1); \
    auto _v2 = (e2); \
    if (!do_assert(_v1 == _v2)) { \
      PRINT_ERROR(); \
      std::cerr << "     Expressions `" #e1 "' and `" #e2 "' are not equal." << std::endl; \
      std::cerr << "     " << #e1 " = " << repr(_v1) << ", " #e2 " = " << repr(_v2) << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_ALMOST_EQUAL(e1, e2, precision) \
  do { \
    auto _v1 = (e1); \
    auto _v2 = (e2); \
    if (!do_assert(std::fabs(_v1 - _v2) < precision)) { \
      PRINT_ERROR(); \
      std::cerr << "     Expressions `" #e1 "' and `" #e2 "' are not almost equal." << std::endl; \
      std::cerr << "     " << #e1 " = " << repr(_v1) << ", " #e2 " = " << repr(_v2) << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_STRINGS_EQUAL(e1, e2) \
  do { \
    std::string _v1 = (e1); \
    std::string _v2 = (e2); \
    if (!do_assert(_v1 == _v2)) { \
      PRINT_ERROR(); \
      std::cerr << "     Strings `" #e1 "' (1) and `" #e2 "' (2) are not equal." << std::endl; \
      std::cerr << "     (1): '" << _v1 << "'," << std::endl; \
      std::cerr << "     (2): '" << _v2 << "'"  << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_FLOATS_EQUAL(e1, e2) \
  do { \
    auto _v1 = (e1); \
    auto _v2 = (e2); \
    if (!do_assert(compare_floats(_v1, _v2))) { \
      PRINT_ERROR(); \
      std::cerr << "     Floating point numbers `" #e1 "' and `" #e2 "' are not equal." << std::endl; \
      std::cerr << "     " << #e1 " = " << repr(_v1) << ", " #e2 " = " << repr(_v2) << std::endl; \
      handle_failfast(); \
    } \
  } while (0)
#define ASSERT_THROWS(exception_class, expression) \
  do { \
    bool caught = false; \
    try { \
      (void)(expression); \
    } catch (const exception_class &_exc) { \
      caught = true; \
      break; \
    } \
    if (!do_assert(caught)) { \
      PRINT_ERROR(); \
      std::cerr << "     Exception " << #exception_class << " not thrown." << std::endl; \
      handle_failfast(); \
    } \
  } while(0)

template<typename T>
T repr(const T value) { return value; }

uint16_t repr(const uint8_t value) { return value; }

bool compare_floats(float v1, float v2) {
  return (std::fabs(v1 - v2) * 100000.f <= std::min(fabs(v1), fabs(v2)));
}

bool compare_floats(double v1, double v2) {
  return (std::fabs(v1 - v2) * 1000000000000. <= std::min(fabs(v1), fabs(v2)));
}

struct TestCase {
  unsigned assertions_total;
  unsigned assertions_successful;
  bool failfast;

  bool do_assert(bool condition) {
    ++assertions_total;
    if (condition) {
      ++assertions_successful;
    }
    return condition;
  }

  void handle_failfast() {
    if (failfast) {
      std::cerr << "Exiting immediately because failfast = true." << std::endl;
      exit(1);
    }
  }

  virtual void run() = 0;
  virtual ~TestCase() {}
};

struct TestCaseInfo {
  TestCase *test_case;
  std::string name;
};

static std::vector<TestCaseInfo> storage;

struct TestCaseProcessor {
  TestCaseProcessor(TestCaseInfo info) {
    storage.push_back(info);
  }
};

#ifdef __linux__
void signal_handler(int signum) {
  std::cerr << E_ERROR("Signal occurred") << ": " << strsignal(signum) << std::endl;
  /* Print the backtrace */
  void *buffer[10];
  size_t size = backtrace(buffer, 10);
  backtrace_symbols_fd(buffer, size, STDERR_FILENO);
  /* Now call the default handler */
  signal(signum, SIG_DFL);
  exit(128 + signum);
}
#endif

void print_help(const char *command_name) {
  std::cout << "Usage: " << command_name << " [-f] [-n]" << std::endl;
  std::cout << std::endl;
  std::cout << "  -f, --fail-fast: Exit after first failure" << std::endl;
  std::cout << "  -n, --no-catch:  Do not catch C++ exceptions (useful for debugging)" << std::endl;
}

int main(int argc, char **argv) {
  bool failfast = false;
  bool nocatch = false;

  for (int i = 1; i < argc; ++i) {
    char *arg = argv[i];
    if (!strcmp(arg, "--fail-fast") || !strcmp(arg, "-f")) {
      failfast = true;
    } else if (!strcmp(arg, "--no-catch") || !strcmp(arg, "-n")) {
      nocatch = true;
    } else {
      print_help(argv[0]);
      return 0;
    }
  }

#ifdef __USE_GNU
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
#endif
#ifdef __linux__
  signal(SIGSEGV, signal_handler);
#endif

  int result = 0;
  for (const TestCaseInfo &info: storage) {
    std::cout << " * " << info.name << std::endl;
    TestCase *test_case = info.test_case;
    test_case->failfast = failfast;
    bool success = true;
    if (nocatch) {
      test_case->run();
    } else {
      try {
        test_case->run();
      } catch (const std::exception &e) {
        success = false;
        std::cerr << "   " << E_ERROR("Exception occurred") << ": " << e.what() << std::endl;
      }
    }
    if (test_case->assertions_successful < test_case->assertions_total) {
      success = false;
    }
    if (success) {
      std::cout << "   Result: " << O_SUCCESS("SUCCESS")
                << " (" << test_case->assertions_total << " assertions passed)"
                << std::endl;
    } else {
      std::cout << "   Result: " << O_ERROR("FAIL")
                << " (" << test_case->assertions_successful << " of "
                << test_case->assertions_total << " assertions passed)"
                << std::endl;
      result = 1;
    }
    delete test_case;
  }
  return result;
}

#endif /* CXXUNIT_TESTING_HPP */
