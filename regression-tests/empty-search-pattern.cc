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
    const char* input = "${HOME:s/$EMPTY/test/}";
    char*    tmp;
    size_t   tmp_len;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("EMPTY", "", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    try
        {
        var_expand(input, strlen(input),
                   &tmp, &tmp_len,
                   &env_lookup, NULL,
                   NULL);
        }
    catch(const empty_search_string&)
        {
        }

    return 0;
    }
