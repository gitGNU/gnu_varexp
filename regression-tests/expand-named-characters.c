#include "../internal.h"

struct test_case
    {
    const char* input;
    const char* expected;
    var_rc_t    rc;
    };

int main(int argc, char** argv)
    {
    struct test_case tests[] =
        {
        { "",                     "",                 VAR_OK                         },
        { "\\",                   NULL,               VAR_INCOMPLETE_NAMED_CHARACTER },
        { "hello world",          "hello world",      VAR_OK                         },
        { "\\n",                  "\n",               VAR_OK                         },
        {"\\t",                   "\t",               VAR_OK                         },
        { "\\rhello\\tworld\\n",  "\rhello\tworld\n", VAR_OK                         },
        { "\\x5a\\x5A",           "ZZ",               VAR_OK                         },
        { "\\x5g\\x5A",           NULL,               VAR_INVALID_HEX                },
        { "\\x5",                 NULL,               VAR_INCOMPLETE_HEX             },
        { "\\033",                "\033",             VAR_OK                         },
        { "\\03",                 NULL,               VAR_INCOMPLETE_OCTAL           },
        { "\\038",                NULL,               VAR_INVALID_OCTAL              },
        { "\\400",                NULL,               VAR_OCTAL_TOO_LARGE            },
        { "\\x{4243}",            "BC",               VAR_OK                         },
        { "\\x{}",                "",                 VAR_OK                         },
        { "\\x{5a5A5a5A}",        "ZZZZ",             VAR_OK                         },
        { "\\x{",                 NULL,               VAR_INCOMPLETE_GROUPED_HEX     },
        { "x\\x{5a5A5a5A}a",      "xZZZZa",           VAR_OK                         },
        { "x\\x{5a5A5a\\0015A}a", NULL,               VAR_INVALID_HEX                },
        { "x\\x{5a\\x{5a}5A}a",   NULL,               VAR_INVALID_HEX                }
        };
    size_t i;
    char tmp[1024];

    for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
        {
        if (expand_named_characters(tests[i].input, strlen(tests[i].input), tmp) != tests[i].rc ||
            (tests[i].expected != NULL && strcmp(tmp, tests[i].expected) != 0))
            {
            printf("expand_named_characters() failed test case %d.\n", i);
            return 1;
            }
        }

    return 0;
    }
