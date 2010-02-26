/*
 * Copyright (c) 2002-2010 Peter Simons <simons@cryp.to>
 * Copyright (c) 2001 The OSSP Project <http://www.ossp.org/>
 * Copyright (c) 2001 Cable & Wireless Deutschland <http://www.cw.com/de/>
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <cstdio>
#include "../internal.hpp"

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
