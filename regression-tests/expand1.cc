#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hh"
using namespace varexp;
using namespace std;

struct dummy : public callback_t
    {
    virtual void operator()(const string& name, string& data)
        {
        throw runtime_error("Not implemented.");
        }
    virtual void operator()(const string& name, int idx, string& data)
        {
        throw array_lookups_are_unsupported();
        }
    };

int main(int argc, char** argv)
    {
    const char* input  = "This is a \\$test!";
    const char* output = "This is a \\$test!";
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
