#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hh"
using namespace varexp;

struct env_lookup : public callback_t
    {
    virtual void operator()(const std::string& name, std::string& data)
        {
        const char* p = getenv(name.c_str());
        if (p == NULL)
            throw undefined_variable();
        else
            data = p;
        }
    virtual void operator()(const std::string& name, int idx, std::string& data)
        {
        throw std::runtime_error("Not implemented.");
        }
    };

int main(int argc, char** argv)
    {
    const char* input = "${HOME:s/$EMPTY/test/}";
    std::string tmp;
    env_lookup lookup;

    if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
        setenv("EMPTY", "", 1) != 0)
        {
        printf("Failed to set the environment: %s.\n", strerror(errno));
        return 1;
        }
    try
        {
        expand(input, tmp, lookup);
        return 1;
        }
    catch(const empty_search_string&)
        {
        }

    return 0;
    }
