#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        var_rc_t lookup_wrapper(void* context,
                                const char* name, size_t name_len, int idx,
                                const char** data, size_t* data_len,
                                size_t* buffer_size)
            {
            static char buf[1];
            struct wrapper_context* wcon = static_cast<wrapper_context*>(context);
            var_rc_t rc;

            try
                {
                rc = (*wcon->lookup)(wcon->context, name, name_len,
                                 idx, data, data_len, buffer_size);
                }
            catch(const undefined_variable&)
                {
                (*wcon->rel_lookup_flag)--;
                *data = buf;
                *data_len = 0;
                *buffer_size = 0;
                rc = 0;
                }
            return rc;
            }

        size_t loop_limits(const char* begin, const char* end,
                           const var_config_t* config,
                           const char_class_t nameclass,
                           var_cb_t lookup, void* lookup_context,
                           int* start, int* step, int* stop, int* open_end)
            {
            const char* p = begin;
            int rc;
            int dummy;

            if (begin == end)
                return 0;

            if (*p != config->startdelim)
                return 0;
            else
                ++p;

            /* Read start value for the loop. */

            try
                {
                rc = num_exp(p, end, 0, start, &dummy,
                             config, nameclass, lookup, lookup_context);
                }
            catch(const invalid_char_in_index_spec&)
                {
                *start = 0;          /* use default */
                }
            p += rc;

            if (*p != ',')
                throw invalid_char_in_loop_limits();
            else
                ++p;

            /* Read step value for the loop. */

            try
                {
                rc = num_exp(p, end, 0, step, &dummy,
                             config, nameclass, lookup, lookup_context);
                p += rc;
                }
            catch(const invalid_char_in_index_spec&)
                {
                *step = 1;          /* use default */
                }

            if (*p != ',')
                {
                if (*p != config->enddelim)
                    throw invalid_char_in_loop_limits();
                else
                    {
                    ++p;
                    *stop = *step;
                    *step = 1;
                    if (rc > 0)
                        *open_end = 0;
                    else
                        *open_end = 1;
                    return p - begin;
                    }
                }
            else
                ++p;

            /* Read stop value for the loop. */

            try
                {
                rc = num_exp(p, end, 0, stop, &dummy,
                             config, nameclass, lookup, lookup_context);
                *open_end = 0;
                p += rc;
                }
            catch(const invalid_char_in_index_spec& e)
                {
                *stop = 0;          /* use default */
                *open_end = 1;
                }

            if (*p != config->enddelim)
                throw invalid_char_in_loop_limits();

            return ++p - begin;
            }
        }
    }
