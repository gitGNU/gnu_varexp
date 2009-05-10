#include <cstdio>
#include "varexp/internal.hpp"
using namespace varexp;
using namespace varexp::internal;

static void class2string(char char_class[256], char* buf)
{
  size_t i;
  for (i = 0; i < 256; ++i)
  {
    if (char_class[i])
      *buf++ = (char)i;
  }
  *buf = '\0';
}

struct test_case
{
  const char* input;
  const char* expected;
};

int main(int argc, char** argv)
{
  struct test_case tests[] =
    {
      { "",               ""                             },
      { "abcabc",         "abc"                          },
      { "a-z",            "abcdefghijklmnopqrstuvwxyz"   },
      { "a-eA-Eabcdef-",  "-ABCDEabcdef"                 },
      { "-a-eA-Eabcdef-", "-ABCDEabcdef"                 },
      { "0-9-",           "-0123456789"                  },
    };
  size_t i;
  char char_class[256];
  char tmp[1024];

  for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
  {
    expand_character_class(tests[i].input, char_class);
    if (tests[i].expected != NULL)
    {
      class2string(char_class, tmp);
      if (strcmp(tmp, tests[i].expected) != 0)
      {
        printf("expand_character_class() failed test case %d.\n", i);
        return 1;
      }
    }
  }

  try
  {
    expand_character_class("g-a", char_class);
    return 1;
  }
  catch(const incorrect_class_spec&)
  {
  }

  return 0;
}
