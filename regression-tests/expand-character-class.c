#include <stdio.h>
#include <string.h>
#include "../internal.h"

static void class2string(char class[256], char* buf)
    {
    size_t i;
    for (i = 0; i < 256; ++i)
        {
        if (class[i])
            *buf++ = (char)i;
        }
    *buf = '\0';
    }

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
        { "",               "",                             VAR_OK                   },
        { "abcabc",         "abc",                          VAR_OK                   },
        { "a-z",            "abcdefghijklmnopqrstuvwxyz",   VAR_OK                   },
        { "a-eA-Eabcdef-",  "-ABCDEabcdef",                 VAR_OK                   },
        { "0-9-",           "-0123456789",                  VAR_OK                   },
        { "g-a",            NULL,                           VAR_INCORRECT_CLASS_SPEC }
        };
    size_t i;
    char class[256];
    char tmp[1024];

    for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
        {
        if (expand_character_class(tests[i].input, class) != tests[i].rc)
            {
            printf("expand_character_class() failed test case %d.\n", i);
            return 1;
            }
        if (tests[i].expected != NULL)
            {
            class2string(class, tmp);
            if (strcmp(tmp, tests[i].expected) != 0)
                {
                printf("expand_character_class() failed test case %d.\n", i);
                return 1;
                }
            }
        }

    return 0;
    }
