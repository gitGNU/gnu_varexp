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

int main(int argc, char** argv)
    {
    const char* input =                                                    \
        "\\$HOME                      = '${HOME}'\\n"                      \
        "\\$FOO                       = '${FOO}'\\n"                       \
        "\\$BAR                       = '${BAR}'\\n"                       \
        "\\$OSTYPE                    = '${${FOO:u}${BAR:u}:l}'\\n"        \
        "\\$UNDEFINED                 = '${UNDEFINED}'\\n"                 \
        "\\${OSTYPE:#}                = '${OSTYPE:#}'\\n"                  \
        "\\${HEINZ:-test\\${FOO}test}   = '${HEINZ:-test${FOO}test}'\\n"   \
        "\\${HEINZ:-test\\${FOO:u}test} = '${HEINZ:-test${FOO:u}test}'\\n" \
        "\\${TERM:-test\\${FOO}test}    = '${TERM:-test${FOO}test}'\\n"    \
        "\\${HEINZ:+FOO}              = '${HEINZ:+FOO}'\\n"                \
        "\\${HOME:+test\\${FOO}test}    = '${HOME:+test${FOO}test}'\\n"    \
        "\\${HOME:+OS\\${BAR:u}}        = '${HOME:+OS${BAR:u}}'\\n"        \
        "\\$TERM                      = '${TERM}'\\n";
    const char* output =                                                 \
        "$HOME                      = '/home/regression-tests'\n"        \
        "$FOO                       = 'os'\n"                            \
        "$BAR                       = 'type'\n"                          \
        "$OSTYPE                    = 'regression-os'\n"                 \
        "$UNDEFINED                 = '${UNDEFINED}'\n"                  \
        "${OSTYPE:#}                = '13'\n"                            \
        "${HEINZ:-test${FOO}test}   = 'testostest'\n"                    \
        "${HEINZ:-test${FOO:u}test} = 'testOStest'\n"                    \
        "${TERM:-test${FOO}test}    = 'regression-term'\n"               \
        "${HEINZ:+FOO}              = ''\n"                              \
        "${HOME:+test${FOO}test}    = '${testostest}'\n"                 \
        "${HOME:+OS${BAR:u}}        = 'regression-os'\n"                 \
        "$TERM                      = 'regression-term'\n";
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("OSTYPE", "regression-os", 1) != 0 ||
        setenv("TERM", "regression-term", 1) != 0 ||
        setenv("FOO", "os", 1) != 0 ||
        setenv("BAR", "type", 1) != 0 ||
        setenv("HEINZ", "", 1) != 0 ||
        unsetenv("UNDEFINED") != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    NULL,
                    &env_lookup, NULL,
                    NULL, 0);
    if (rc != VAR_OK)
        {
        printf("var_expand() failed with error %d.\n", rc);
        return 1;
        }

    rc = expand_named_characters(tmp, tmp_len, tmp);
    if (rc != VAR_OK)
        {
        printf("expand_named_characters() failed with error %d.\n", rc);
        return 1;
        }
    else
        tmp_len = strlen(tmp);

    printf("==================================================\n%s==================================================\n", tmp);

    if (tmp_len != strlen(output))
        {
        printf("The length of the output string is not what we expected: %d != %d.\n",
               tmp_len, strlen(output));
        return 1;
        }

    if (memcmp(tmp, output, tmp_len) != 0)
        {
        printf("The buffer returned by var_expand() is not what we expected.\n");
        return 1;
        }

    return 0;
    }