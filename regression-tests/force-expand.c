#include "../internal.h"

int dummy(void* context,
          const char* varname, size_t name_len,
          const char** data, size_t* data_len, size_t* buffer_size)
    {
    if (name_len != sizeof("heinz_ist_doof")-1)
        {
        printf("The the length of the variable name (%d) doesn't fit.\n", name_len);
        exit(1);
        }
    if (memcmp(varname, "heinz_ist_doof", sizeof("heinz_ist_doof")-1) != 0)
        {
        printf("Callback called for unknown variable.\n");
        exit(1);
        }
    return 0;                   /* say it's undefined */
    }

int main(int argc, char** argv)
    {
    const char* input  = "This is a $heinz_ist_doof!";
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    /* Run var_expand() with force_expand and expect failure. */

    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    &dummy, NULL,
                    NULL, 1);
    if (rc != VAR_UNDEFINED_VARIABLE)
        {
        printf("var_expand() should have failed with error UNDEFINED_VARIABLE, but returned %d.\n", rc);
        return 1;
        }

    /* Run var_expand() without force_expand and expect input == output. */

    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    &dummy, NULL,
                    NULL, 0);
    if (rc != VAR_OK)
        {
        printf("var_expand() failed with error %d.\n", rc);
        return 1;
        }

    if (tmp_len != strlen(input))
        {
        printf("The length of the input string is not what we expected: %d != %d.\n",
               tmp_len, strlen(input));
        return 1;
        }

    if (memcmp(tmp, input, tmp_len) != 0)
        {
        printf("The buffer returned by var_expand() is not what we expected.\n");
        return 1;
        }

    return 0;
    }
