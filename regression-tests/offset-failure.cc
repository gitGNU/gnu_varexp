#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hh"
using namespace varexp;

int env_lookup(void* context,
               const char* varname, size_t name_len, int idx,
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

    if (setenv("HOME", "/home/regression-tests", 1) !=0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }

    try
        {
        var_expand(input1, strlen(input1), &tmp, &tmp_len, &env_lookup, NULL, NULL);
        }
    catch(const offset_out_of_bounds&)
        {
        }
    try
        {
        var_expand(input2, strlen(input2), &tmp, &tmp_len, &env_lookup, NULL, NULL);
        }
    catch(const offset_out_of_bounds&)
        {
        }
    try
        {
        var_expand(input3, strlen(input3), &tmp, &tmp_len, &env_lookup, NULL, NULL);
        }
    catch(const range_out_of_bounds&)
        {
        }
    try
        {
        var_expand(input4, strlen(input4), &tmp, &tmp_len, &env_lookup, NULL, NULL);
        }
    catch(const range_out_of_bounds&)
        {
        }
    try
        {
        var_expand(input5, strlen(input5), &tmp, &tmp_len, &env_lookup, NULL, NULL);
        }
    catch(const offset_logic&)
        {
        }

    return 0;
    }
