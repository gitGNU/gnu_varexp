#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hh"
using namespace varexp;

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
        throw undefined_variable();
    *data_len = strlen(*data);
    *buffer_size = 0;
    return var_rc_t(1);
    }

int main(int argc, char** argv)
    {
    const char* input =                            \
        "\\$HOME      = '${HOME}'\\n"              \
        "\\$OSTYPE    = '${OSTYPE}'\\n"            \
        "\\$TERM      = '${TERM}'\\n";
    const char* output =                           \
        "$HOME      = '/home/regression-tests'\n"  \
        "$OSTYPE    = 'regression-os'\n"           \
        "$TERM      = 'regression-term'\n";
    char*    tmp;
    size_t   tmp_len;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("OSTYPE", "regression-os", 1) != 0 ||
        setenv("TERM", "regression-term", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    unsetenv("UNDEFINED");
    var_expand(input, strlen(input),
               &tmp, &tmp_len,
               &env_lookup, NULL,
               NULL);

    var_unescape(tmp, tmp_len, tmp, 1);
    tmp_len = strlen(tmp);
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
