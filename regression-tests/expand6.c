#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../internal.h"
#include <dmalloc.h>

int env_lookup(void* context,
          const char* varname, size_t name_len,
          const char** data, size_t* data_len, size_t* buffer_size)
    {
    char tmp[256];

    if (name_len > sizeof(tmp)-1)
        {
        printf("Callback can't expand variable names longer than %d characters.\n", sizeof(tmp-1));
        exit(1);
        }
    memcpy(tmp, varname, name_len);
    tmp[name_len] = '\0';
    *data = getenv(tmp);
    if (*data == NULL)
        return 0;
    *data_len = strlen(*data);
    *buffer_size = 0;
    return 1;
    }

struct test_case
    {
    const char* input;
    const char* expected;
    };

int main(int argc, char** argv)
    {
    const struct test_case tests[] =
        {
        { "$HOME",                      "/home/regression-tests"          },
        { "${FOO}",                     "os"                              },
        { "${BAR}",                     "type"                            },
        { "${${FOO:u}${BAR:u}:l}",      "regression-os"                   },
        { "${UNDEFINED}",               "${UNDEFINED}"                    },
        { "${OSTYPE:#}",                "13"                              },
        { "${EMPTY:-test${FOO}test}",   "testostest"                      },
        { "${EMPTY:-test${FOO:u}test}", "testOStest"                      },
        { "${TERM:-test${FOO}test}",    "regression-term"                 },
        { "${EMPTY:+FOO}",              ""                                },
        { "${HOME:+test${FOO}test}",    "${testostest}"                   },
        { "${HOME:+OS${BAR:u}}",        "regression-os"                   },
        { "${HOME:+OS${UNDEFINED:u}}",  "${OS${UNDEFINED:u}}"             },
        { "${UNDEFINED:+OS${BAR:u}}",   "${UNDEFINED:+OS${BAR:u}}"        },
        { "${HOME:*heinz}",             ""                                },
        { "${EMPTY:*claus}",            "claus"                           },
        { "${TERM}",                    "regression-term"                 },
        { "${HOME:s/reg/bla/}",         "/home/blaression-tests"          },
        { "${HOME:s/e/bla/}",           "/hombla/regression-tests"        },
        { "${HOME:s/e/bla/g}",          "/hombla/rblagrblassion-tblasts"  },
        { "${HOME:s/\\//_/g}",          "_home_regression-tests"          },
        { "${HOME:s/[eso]/_/g}",        "/h_m_/r_gr___i_n-t__t_"          },
        { "${HOME:s/[esO]/_/g}",        "/hom_/r_gr___ion-t__t_"          },
        { "${HOME:s/[esO]/_/gi}",       "/h_m_/r_gr___i_n-t__t_"          },
        { "${OSTYPE:s/^[re]/_/g}",      "_egression-os"                   },
        { "${EMPTY:s/^[re]/_/g}",       ""                                },
        { "${HOME:s/.*/heinz/}",        "heinz"                           },
        { "${HOME:s/e/bla/t}",          "/hombla/regression-tests"        },
        { "${HOME:s/E/bla/t}",          "/home/regression-tests"          },
        { "${HOME:s/E/bla/ti}",         "/hombla/regression-tests"        },
        { "${HOME:s/E/bla/tig}",        "/hombla/rblagrblassion-tblasts"  },
        { "${HOME:o1-5}",               "home/"                           },
        { "${HOME:o1,5}",               "home"                            },
        { "${HOME:o5,}",                "/regression-tests"               },
        { "${HOME:o5-}",                "/regression-tests"               },
        { "${HOME:o7,13}",              "egress"                          }
        };
    /*
        { "${HOME:s/g(res)s/x\\\\1x/g}","/homE/rEgrEssion-tEsts"          }
        { "${HOME:s/\\x65/\\x45/g}",    "/home/regression-tests"          }
    */
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;
    size_t   i;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("OSTYPE", "regression-os", 1) != 0 ||
        setenv("TERM", "regression-term", 1) != 0 ||
        setenv("FOO", "os", 1) != 0 ||
        setenv("BAR", "type", 1) != 0 ||
        setenv("EMPTY", "", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    unsetenv("UNDEFINED");

    for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
        {
        rc = var_expand(tests[i].input, strlen(tests[i].input),
                        &tmp, &tmp_len, NULL,
                        &env_lookup, NULL,
                        NULL, 0);
        if (rc != VAR_OK)
            {
            printf("Test case #%d: var_expand() failed with return code %d.\n", i, rc);
            return 1;
            }
        if (tmp_len != strlen(tests[i].expected) || tmp == NULL || memcmp(tests[i].expected, tmp, tmp_len) != 0)
            {
            printf("Test case #%d: Expected result '%s' but got '%s'.\n", i, tests[i].expected, tmp);
            return 1;
            }
        printf("Test case #%02d: '%s' --> '%s'.\n", i, tests[i].input, tmp);
        }

    return 0;
    }
