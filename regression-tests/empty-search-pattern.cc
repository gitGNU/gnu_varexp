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
    const char* input = "${HOME:s/$EMPTY/test/}";
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("EMPTY", "", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    &env_lookup, NULL,
                    NULL);
    if (rc != VAR_ERR_EMPTY_SEARCH_STRING)
        {
        printf("var_expand() should have failed with VAR_ERR_EMPTY_SEARCH_STRING but returned %d.\n", rc);
        return 1;
        }

    return 0;
    }
