#include "../internal.hh"

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
        { "-a-eA-Eabcdef-", "-ABCDEabcdef",                 VAR_OK                   },
        { "0-9-",           "-0123456789",                  VAR_OK                   },
        { "g-a",            NULL,                           VAR_ERR_INCORRECT_CLASS_SPEC }
        };
    size_t i;
    char char_class[256];
    char tmp[1024];

    for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
        {
        if (expand_character_class(tests[i].input, char_class) != tests[i].rc)
            {
            printf("expand_character_class() failed test case %d.\n", i);
            return 1;
            }
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

    return 0;
    }
