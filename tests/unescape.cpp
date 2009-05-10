#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hpp"

using namespace varexp;
using namespace std;

struct test_case
{
  const char* input;
  const char* expected;
};

int main(int argc, char** argv)
{
  struct test_case tests[] =
    {
      { "",                     ""                 },
      { "hello world",          "hello world"      },
      { "\\n",                  "\n"               },
      {"\\t",                   "\t"               },
      { "\\rhello\\tworld\\n",  "\rhello\tworld\n" },
      { "\\x5a\\x5A",           "ZZ"               },
      { "\\033",                "\033"             },
      { "\\x{4243}",            "BC"               },
      { "\\x{}",                ""                 },
      { "\\x{5a5A5a5A}",        "ZZZZ"             },
      { "x\\x{5a5A5a5A}a",      "xZZZZa"           },
    };
  size_t i;
  string tmp;

  for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
  {
    unescape(tests[i].input, tmp, false);
    if (tmp != tests[i].expected)
    {
      printf("unescape() failed test case %d.\n", i);
      return 1;
    }
  }

#define fail_test(input, exception)            \
    try                                        \
        {                                      \
        unescape(input, tmp, false);           \
        return 1;                              \
        }                                      \
    catch(const exception&)                    \
        {                                      \
        }                                      \
        while(false)

  fail_test("\\x5g\\x5A",           invalid_hex);
  fail_test("\\x5",                 incomplete_hex);
  fail_test("\\038",                invalid_octal);
  fail_test("\\400",                octal_too_large);
  fail_test("\\x{",                 incomplete_grouped_hex);
  fail_test("x\\x{5a5A5a\\0015A}a", invalid_hex);
  fail_test("x\\x{5a\\x{5a}5A}a",   invalid_hex);

  return 0;
}
