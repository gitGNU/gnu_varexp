#include "../internal.h"

int env_lookup(void* context,
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
    return 1;
    }

int main(int argc, char** argv)
    {
    const char* input1 = "${HOME:o88,}";
    const char* input2 = "${HOME:o88-90}";
    const char* input3 = "${HOME:o8-90}";
    const char* input4 = "${HOME:o8,90}";
    const char* input5 = "${HOME:o8,4}";

    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    if (setenv("HOME", "/home/regression-tests", 1) !=0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    if ((rc = var_expand(input1, strlen(input1), &tmp, &tmp_len, &env_lookup, NULL, NULL)) != VAR_ERR_OFFSET_OUT_OF_BOUNDS)
        {
        printf("var_expand() should have failed with VAR_ERR_OFFSET_OUT_OF_BOUNDS but returned %d.\n", rc);
        return 1;
        }
    if ((rc = var_expand(input2, strlen(input2), &tmp, &tmp_len, &env_lookup, NULL, NULL)) != VAR_ERR_OFFSET_OUT_OF_BOUNDS)
        {
        printf("var_expand() should have failed with VAR_ERR_OFFSET_OUT_OF_BOUNDS but returned %d.\n", rc);
        return 1;
        }
    if ((rc = var_expand(input3, strlen(input3), &tmp, &tmp_len, &env_lookup, NULL, NULL)) != VAR_ERR_RANGE_OUT_OF_BOUNDS)
        {
        printf("var_expand() should have failed with VAR_ERR_RANGE_OUT_OF_BOUNDS but returned %d.\n", rc);
        return 1;
        }
    if ((rc = var_expand(input4, strlen(input4), &tmp, &tmp_len, &env_lookup, NULL, NULL)) != VAR_ERR_RANGE_OUT_OF_BOUNDS)
        {
        printf("var_expand() should have failed with VAR_ERR_RANGE_OUT_OF_BOUNDS but returned %d.\n", rc);
        return 1;
        }
    if ((rc = var_expand(input5, strlen(input5), &tmp, &tmp_len, &env_lookup, NULL, NULL)) != VAR_ERR_OFFSET_LOGIC)
        {
        printf("var_expand() should have failed with VAR_ERR_OFFSET_LOGIC but returned %d.\n", rc);
        return 1;
        }

    return 0;
    }
