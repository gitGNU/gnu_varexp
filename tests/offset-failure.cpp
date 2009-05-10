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
  const char* input1 = "${HOME:o88,}";
  const char* input2 = "${HOME:o88-90}";
  const char* input3 = "${HOME:o8-90}";
  const char* input4 = "${HOME:o8,90}";
  const char* input5 = "${HOME:o8,4}";

  string tmp;
  env_lookup lookup;

  if (setenv("HOME", "/home/regression-tests", 1) !=0)
  {
    printf("Failed to set the environment: %s.\n", strerror(errno));
    return 1;
  }

#define fail_test(input, exception)            \
    try                                        \
    {                                          \
      expand(input, tmp, lookup);              \
      return 1;                                \
    }                                          \
    catch(exception const &)                   \
    {                                          \
    }                                          \
    while(false)

  fail_test(input1, offset_out_of_bounds);
  fail_test(input2, offset_out_of_bounds);
  fail_test(input3, range_out_of_bounds);
  fail_test(input4, range_out_of_bounds);
  fail_test(input5, offset_logic);

  return 0;
}
