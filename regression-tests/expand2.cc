#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hh"
using namespace varexp;

struct dummy : public callback_t
    {
    virtual void operator()(const std::string& name, std::string& data)
        {
        if (name == "test")
            data = "foobar";
        else
            throw undefined_variable();
        }
    virtual void operator()(const std::string& name, int idx, std::string& data)
        {
        throw std::runtime_error("Not implemented.");
        }
    };

int main(int argc, char** argv)
    {
    const char* input  = "This is a $test!";
    const char* output = "This is a foobar!";
    std::string tmp;
    dummy lookup;

    expand(input, tmp, lookup);
    if (tmp != output)
        {
        printf("The buffer returned by var_expand() is not what we expected.\n");
        return 1;
        }

    return 0;
    }
