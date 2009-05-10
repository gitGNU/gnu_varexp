#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../varexp.hpp"

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
  const char* input =                          \
    "\\$HOME      = '${HOME}'\\n"              \
    "\\$OSTYPE    = '${OSTYPE}'\\n"            \
    "\\$TERM      = '${TERM}'\\n";
  const char* output =                         \
    "$HOME      = '/home/regression-tests'\n"  \
    "$OSTYPE    = 'regression-os'\n"           \
    "$TERM      = 'regression-term'\n";
  string tmp;
  env_lookup lookup;

  if (setenv("HOME", "/home/regression-tests", 1) != 0 ||
     setenv("OSTYPE", "regression-os", 1) != 0 ||
     setenv("TERM", "regression-term", 1) != 0)
  {
    printf("Failed to set the environment: %s.\n", strerror(errno));
    return 1;
  }
  unsetenv("UNDEFINED");

  expand(input, tmp, lookup);
  unescape(tmp, tmp, true);

  if (tmp != output)
  {
    printf("The buffer returned by var_expand() is not what we expected.\n");
    return 1;
  }

  return 0;
}
