#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t ascii_to_uint(const char* begin, const char* end, unsigned int& result)
            {
            const char* p = begin;
            result = 0;

            while (isdigit(*p) && p != end)
                {
                result *= 10;
                result += *p - '0';
                ++p;
                }
            return p - begin;
            }
        }
    }
