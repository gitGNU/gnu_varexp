#include "../internal.h"

var_rc_t env_lookup(void* context,
               const char* varname, size_t name_len, int index,
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
        return VAR_ERR_UNDEFINED_VARIABLE;
    *data_len = strlen(*data);
    *buffer_size = 0;
    return var_rc_t(1);
    }

int main(int argc, char** argv)
    {
    const char* input =                            \
        "\\$HOME      = '${HOME}'\\n"              \
        "\\$OSTYPE    = '${$FOO${BAR}}'\\n"        \
        "\\$TERM      = '${TERM}'\\n";
    const char* output =                           \
        "$HOME      = '/home/regression-tests'\n"  \
        "$OSTYPE    = 'regression-os'\n"           \
        "$TERM      = 'regression-term'\n";
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("OSTYPE", "regression-os", 1) != 0 ||
        setenv("TERM", "regression-term", 1) != 0 ||
        setenv("FOO", "OS", 1) != 0 ||
        setenv("BAR", "TYPE", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    unsetenv("UNDEFINED");
    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    &env_lookup, NULL,
                    NULL);
    if (rc != VAR_OK)
        {
        printf("var_expand() failed with error %d.\n", rc);
        return 1;
        }

    rc = var_unescape(tmp, tmp_len, tmp, 1);
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
