#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../internal.h"

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
    var_rc_t    rc;
    };

int main(int argc, char** argv)
    {
    const struct test_case tests[] =
        {
        { "$HOME",                      "/home/regression-tests",         VAR_OK },
        { "${FOO}",                     "os",                             VAR_OK },
        { "${BAR}",                     "type",                           VAR_OK },
        { "${${FOO:u}${BAR:u}:l}",      "regression-os",                  VAR_OK },
        { "${UNDEFINED}",               "${UNDEFINED}",                   VAR_OK },
        { "${OSTYPE:#}",                "13",                             VAR_OK },
        { "${EMPTY:-test${FOO}test}",   "testostest",                     VAR_OK },
        { "${EMPTY:-test${FOO:u}test}", "testOStest",                     VAR_OK },
        { "${TERM:-test${FOO}test}",    "regression-term",                VAR_OK },
        { "${EMPTY:+FOO}",              "",                               VAR_OK },
        { "${HOME:+test${FOO}test}",    "${testostest}",                  VAR_OK },
        { "${HOME:+OS${BAR:u}}",        "regression-os",                  VAR_OK },
        { "${HOME:+OS${UNDEFINED:u}}",  "${OS${UNDEFINED:u}}",            VAR_OK },
        { "${UNDEFINED:+OS${BAR:u}}",   "${UNDEFINED:+OS${BAR:u}}",       VAR_OK },
        { "${HOME:*heinz}",             "",                               VAR_OK },
        { "${EMPTY:*claus}",            "claus",                          VAR_OK },
        { "${TERM}",                    "regression-term",                VAR_OK },
        { "${HOME:s/reg/bla/}",         "/home/blaression-tests",         VAR_OK },
        { "${HOME:s/e/bla/g}",          "/hombla/rblagrblassion-tblasts", VAR_OK }
        };
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;
    size_t   i;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("OSTYPE", "regression-os", 1) != 0 ||
        setenv("TERM", "regression-term", 1) != 0 ||
        setenv("FOO", "os", 1) != 0 ||
        setenv("BAR", "type", 1) != 0 ||
        setenv("EMPTY", "", 1) != 0 ||
        unsetenv("UNDEFINED") != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }

    for (i = 0; i < sizeof(tests) / sizeof(struct test_case); ++i)
        {
        rc = var_expand(tests[i].input, strlen(tests[i].input),
                        &tmp, &tmp_len, NULL,
                        &env_lookup, NULL,
                        NULL, 0);
        if (rc != tests[i].rc)
            {
            printf("Test case #%d: Expected return code %d but got %d.\n", i, tests[i].rc, rc);
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
