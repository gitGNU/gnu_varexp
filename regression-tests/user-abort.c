#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../internal.h"
#include <dmalloc.h>

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
    const char* input = "${EMPTY:?\\$EMPTY soll leer sein}\n";
    char*    tmp;
    size_t   tmp_len;
    var_rc_t rc;

    if (setenv("EMPTY", "", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    rc = var_expand(input, strlen(input),
                    &tmp, &tmp_len,
                    NULL,
                    &env_lookup, NULL,
                    NULL, 0);
    if (rc != VAR_USER_ABORT)
        {
        printf("var_expand() should have failed with VAR_USER_ABORT but returned %d.\n", rc);
        return 1;
        }

    return 0;
    }
