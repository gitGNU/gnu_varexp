#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t parser::variable(const char *begin, const char *end, std::string& result)
            {
            const char* p = begin;
            size_t rc;

            // Expect VARINIT.

            if (p == end || *p != config.varinit)
                return 0;

            if (++p == end)
                throw incomplete_variable_spec();

            // Try to read the variable name. If that fails, we're
            // parsing a complex expression.

            rc = varname(p, end);
            if (rc > 0)
                {
                (*lookup)(string(p, rc), result);
                return 1 + rc;
                }

            // OK, we're dealing with a complex expression here.

            rc = expression(p, end, result);
            if (rc > 0)
                return 1 + rc;

            throw incomplete_variable_spec();
            }
        }
    }
