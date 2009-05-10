#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "varexp/varexp.hpp"
using namespace varexp;
using namespace std;

struct env_lookup : public callback_t
{
  virtual void operator()(const string& name, string& data)
  {
    const char* p = getenv(name.c_str());
    if (p == NULL)
      throw undefined_variable();
    else
      data = p;
  }
  virtual void operator()(const string& name, int idx, string& data)
  {
    throw array_lookups_are_unsupported();
  }
};

int main(int argc, char** argv)
{
  const char* input = "${HOME:s/$EMPTY/test/}";
  string tmp;
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
