#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "varexp/varexp.hpp"
using namespace varexp;
using namespace std;

struct dummy : public callback_t
    {
    virtual void operator()(const string& name, string& data)
        {
        if (name == "test")
            data = "foobar";
        else
            throw undefined_variable();
        }
    virtual void operator()(const string& name, int idx, string& data)
        {
        throw array_lookups_are_unsupported();
        }
    };

int main(int argc, char** argv)
    {
    const char* input  = "This is a $test!";
    const char* output = "This is a foobar!";
    string tmp;
    dummy lookup;

    expand(input, tmp, lookup);
    if (tmp != output)
        {
        printf("The buffer returned by var_expand() is not what we expected.\n");
        return 1;
        }

    return 0;
    }
