#include "../internal.h"

int dummy(void* context,
          const char* varname, size_t name_len, int index,
          const char** data, size_t* data_len, size_t* buffer_size)
    {
    if (name_len != sizeof("test")-1)
        {
        printf("The the length of the variable name (%d) doesn't fit.\n", name_len);
        exit(1);
        }
    if (memcmp(varname, "test", sizeof("test")-1) != 0)
        {
        printf("Callback called for unknown variable.\n");
        exit(1);
        }
    *data        = "foobar";
    *data_len    = sizeof("foobar")-1;
    *buffer_size = 0;

    return 1;
    }

int main(int argc, char** argv)
    {
    const char* input  = "This is a $test!";
    const char* output = "This is a foobar!";
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    &dummy, NULL,
                    NULL);
    if (rc != VAR_OK)
        {
        printf("var_expand() failed with error %d.\n", rc);
        return 1;
        }

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
