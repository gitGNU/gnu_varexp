#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t parser::loop_limits(const char *begin, const char *end, int& start, int& step, int& stop, bool& open_end)
            {
            const char* p = begin;
            int rc;

            if (begin == end || *p != config.startdelim)
                return 0;
            else
                ++p;

            // Read start value for the loop.

            try
                {
                rc = num_exp(p, end, start);
                p += rc;
                }
            catch(const invalid_char_in_index_spec&)
                {
                start = 0;          // Use default.
                }

            if (*p != ',')
                throw invalid_char_in_loop_limits();
            else
                ++p;

            // Read step value for the loop.

            try
                {
                rc = num_exp(p, end, step);
                p += rc;
                }
            catch(const invalid_char_in_index_spec&)
                {
                step = 1;           // Use default.
                rc = 0;
                }

            if (*p != ',')
                {
                if (*p != config.enddelim)
                    throw invalid_char_in_loop_limits();
                else
                    {
                    ++p;
                    stop = step;
                    step = 1;
                    if (rc > 0)
                        open_end = false;
                    else
                        open_end = true;
                    return p - begin;
                    }
                }
            else
                ++p;

            // Read stop value for the loop.

            try
                {
                rc = num_exp(p, end, stop);
                open_end = false;
                p += rc;
                }
            catch(const invalid_char_in_index_spec& e)
                {
                stop = 0;           // Use default.
                open_end = true;
                }

            if (*p != config.enddelim)
                throw invalid_char_in_loop_limits();
            else
                ++p;

            return p - begin;
            }
        }
    }
