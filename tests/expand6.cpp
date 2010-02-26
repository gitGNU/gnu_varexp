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
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hpp"

using namespace varexp;
using namespace std;

struct variable
{
  char const * name;
  char const * data;
};

struct variable_array
{
  char const * name;
  int const    idx;
  char const * data;
};

class var_lookup : public callback_t
{
public:
  var_lookup(const variable* _vars, const variable_array* _array_vars)
    : vars(_vars), array_vars(_array_vars)
  {
  }
  virtual void operator()(const string& name, string& data)
  {
    for (size_t i = 0; vars[i].name; ++i)
    {
      if (strncmp(name.data(), vars[i].name, name.size()) == 0)
      {
        data = vars[i].data;
        return;
      }
    }
    throw undefined_variable();
  }
  virtual void operator()(const string& name, int idx, string& data)
  {
    static char buf[((sizeof(int)*8)/3)+10]; /* sufficient size: <#bits> x log_10(2) + safety */

    if (idx >= 0)
    {
      for (size_t i = 0; array_vars[i].name; ++i)
      {
        if (strncmp(name.data(), array_vars[i].name, name.size()) == 0 && array_vars[i].idx == idx)
        {
          data = array_vars[i].data;
          return;
        }
      }
    }
    else
    {
      for (size_t i = 0; array_vars[i].name; ++i)
      {
        if (strncmp(name.data(), array_vars[i].name, name.size()) == 0)
        {
          size_t counter = 1;
          size_t length  = strlen(array_vars[i].data);
          while (array_vars[i + counter].data && strncmp(name.data(), array_vars[i + counter].name, name.size()) == 0)
            ++counter;
          if (counter == 1)
            sprintf(buf, "%d", length);
          else
            sprintf(buf, "%d", counter);
          data = buf;
          return;
        }
      }
    }
    throw undefined_variable();
  }
private:
  const variable*       vars;
  const variable_array* array_vars;
};

struct test_case
{
  const char* input;
  const char* expected;
};

int main(int argc, char* *argv)
try
{
  const struct variable vars[] =
    {
      { "BAR",       "type" },
      { "EMPTY",     "" },
      { "FOO",       "os" },
      { "HOME",      "/home/regression-tests" },
      { "NUMBER",    "+2" },
      { "MULTILINE", "line1\nline2\n" },
      { "NUMEXP",    "((16)%5)" },
      { "OSTYPE",    "regression-os" },
      { "TERM",      "regression-term" },
      { NULL,        NULL }
    };
  const struct variable_array array_vars[] =
    {
      { "ARRAY",     0, "entry0" },
      { "ARRAY",     1, "entry1" },
      { "ARRAY",     2, "entry2" },
      { "ARRAY",     3, "entry3" },
      { "HEINZ",     0, "heinz0" },
      { "HEINZ",     1, "heinz1" },
      { NULL,        0, NULL }
    };

  const struct test_case tests[] =
    {
      { "$HOME",                        "/home/regression-tests"                                           },
      { "${FOO}",                       "os"                                                               },
      { "${BAR}",                       "type"                                                             },
      { "${${FOO:u}${BAR:u}:l:u}",      "REGRESSION-OS"                                                    },
      { "${OSTYPE:#}",                  "13"                                                               },
      { "${EMPTY:-test${FOO}test}",     "testostest"                                                       },
      { "${EMPTY:-test${FOO:u}test}",   "testOStest"                                                       },
      { "${TERM:-test${FOO}test}",      "regression-term"                                                  },
      { "${EMPTY:+FOO}",                ""                                                                 },
      { "${HOME:+test${FOO}test}",      "testostest"                                                       },
      { "${HOME:+${OS${BAR:u}}}",       "regression-os"                                                    },
      { "${HOME:*heinz}",               ""                                                                 },
      { "${EMPTY:*claus}",              "claus"                                                            },
      { "${TERM}",                      "regression-term"                                                  },
      { "${HOME:s/reg/bla/}",           "/home/blaression-tests"                                           },
      { "${HOME:s/e/bla/}",             "/hombla/regression-tests"                                         },
      { "${HOME:s/e/bla/g}",            "/hombla/rblagrblassion-tblasts"                                   },
      { "${HOME:s/\\//_/g}",            "_home_regression-tests"                                           },
      { "${HOME:s/[eso]/_/g}",          "/h_m_/r_gr___i_n-t__t_"                                           },
      { "${HOME:s/[esO]/_/g}",          "/hom_/r_gr___ion-t__t_"                                           },
      { "${HOME:s/[esO]/_/gi}",         "/h_m_/r_gr___i_n-t__t_"                                           },
      { "${OSTYPE:s/^[re]/_/g}",        "_egression-os"                                                    },
      { "${EMPTY:s/^[re]/_/g}",         ""                                                                 },
      { "${HOME:s/.*\\x{}/heinz/}",     "heinz"                                                            },
      { "${HOME:s/e/bla/t}",            "/hombla/regression-tests"                                         },
      { "${HOME:s/E/bla/t}",            "/home/regression-tests"                                           },
      { "${HOME:s/E/bla/ti}",           "/hombla/regression-tests"                                         },
      { "${HOME:s/E/bla/tig}",          "/hombla/rblagrblassion-tblasts"                                   },
      { "${HOME:o1-5}",                 "home/"                                                            },
      { "${HOME:o1,5}",                 "home/"                                                            },
      { "${HOME:o5,}",                  "/regression-tests"                                                },
      { "${HOME:o5-}",                  "/regression-tests"                                                },
      { "${HOME:o7,13}",                "egressi"                                                          },
      { "${HOME:y/a-z/A-YZ/}",          "/HOME/REGRESSION-TESTS"                                           },
      { "${HOME:y/e-g/a-c/}",           "/homa/racrassion-tasts"                                           },
      { "${FOO:p/15/../l}",             "os............."                                                  },
      { "${FOO:p/15/12345/l}",          "os1234512345123"                                                  },
      { "${FOO:p/15/../r}",             ".............os"                                                  },
      { "${FOO:p/15/12345/r}",          "1234512345123os"                                                  },
      { "${FOO:p/15/../c}",             "......os......."                                                  },
      { "${FOO:p/15/12345/c}",          "123451os1234512"                                                  },
      { "${FOO:s/os/\\x{4F}\\123/g}",   "OS"                                                               },
      { "${FOO:s/os/\\1\\x4F\\123/t}",  "\\1OS"                                                            },
      { "${HOME:s/g(res)s/x\\1x/g}",    "/home/rexresxion-tests"                                           },
      { "${HOME:s/(s+)/_\\1_/g}",       "/home/regre_ss_ion-te_s_t_s_"                                     },
      { "${HOME:s/\\x65/\\x45/g}",      "/homE/rEgrEssion-tEsts"                                           },
      { "${HOME:s/(s*)/x\\1X/g}",       "xX/xXhxXoxXmxXexX/xXrxXexXgxXrxXexssXxXixXoxXnxX-xXtxXexsXxXtxsX" },
      { "${HOME:s/./\\\\/g}",           "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"                     },
      { "${ARRAY[1]}",                  "entry1",                                                          },
      { "${ARRAY[(5+(3+4)*2)%16]}",     "entry3",                                                          },
      { "${ARRAY[(5+(3+4)*2)/9]}",      "entry2",                                                          },
      { "${ARRAY[+1--2]}",              "entry3"                                                           },
      { "${ARRAY[-1]}",                 "4"                                                                },
      { "${ARRAY[$NUMBER]}",            "entry2"                                                           },
      { "${ARRAY[$NUMEXP]}",            "entry1"                                                           },
      { "${ARRAY[$NUMEXP-1]}",          "entry0"                                                           },
      { "[${ARRAY[#]}-]",               "entry0-entry1-entry2-entry3-"                                     },
      { "[${ARRAY[#+1]}-]",             "entry1-entry2-entry3-"                                            },
      { "-[${ARRAY[#]}:]{1,$NUMBER}-",  "-entry1:entry2:-"                                                 },
      { "-[${ARRAY[#]}:]{1,3,5}-",      "-entry1::-"                                                       },
      { "[${ARRAY[#+#-#]}]",            "entry0entry1entry2entry3"                                         },
      { "[${ARRAY[0]}:${ARRAY[#]}-]",   "entry0:entry0-entry0:entry1-entry0:entry2-entry0:entry3-"         },
      { "[${HEINZ[#]}:${ARRAY[#]}-]",   "heinz0:entry0-heinz1:entry1-:entry2-:entry3-"                     },
      { "[${HEINZ[#]}:[${ARRAY[#]}] ]", "heinz0:entry0entry1entry2entry3 heinz1:entry0entry1entry2entry3 " },
      { "[${ARRAY[#]}:[${ARRAY[#]},]{1,2,} ]{0,2,}", "entry0:entry1,entry3, entry2:entry1,entry3, "        },
      { "${MULTILINE:s/^/ | /g}",       " | line1\n | line2\n" },
      {
        "[${HEINZ[#]}: [${ARRAY[#]}${ARRAY[#+1]:+, }]${HEINZ[#+1]:+; }]",
        "heinz0: entry0, entry1, entry2, entry3; heinz1: entry0, entry1, entry2, entry3"
      }
    };
  var_lookup lookup(vars, array_vars);

  for (size_t i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
  {
    printf("Test case #%02d: Original input is '%s'.\n", i, tests[i].input);

    string tmp;
    unescape(tests[i].input, tmp, false);
    printf("Test case #%02d: Unescaped input is '%s'.\n", i, tmp.c_str());

                   expand(tmp, tmp, lookup);
                   printf("Test case #%02d: Output is '%s'.\n", i, tmp.c_str());

                                  if (tmp.size() != strlen(tests[i].expected) ||
                                     memcmp(tests[i].expected, tmp.data(), tmp.size()) != 0)
                                  {
                                    printf("Test case #%02d: Expected result '%s' but got '%s'.\n", i, tests[i].expected, tmp.c_str());
                                    return 1;
                                  }
  }
  return 0;
}
catch(const exception& e)
{
  printf("Caught exception: %s\n", e.what());
  return 1;
}
catch(...)
{
  printf("Caught unknown exception.");
  return 1;
}
