#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        int variable(const char* begin, const char* end,
                     const var_config_t* config, const char_class_t nameclass,
                     var_cb_t lookup, void* lookup_context,
                     tokenbuf_t* result, int current_index,
                     int* rel_lookup_flag)
            {
            const char* p = begin;
            const char* data;
            size_t len, buffer_size;
            int rc, rc2;

            /* Clear the result tokenbuf to make sure we're in a defined
               state. */

            tokenbuf_init(result);

            /* Expect VARINIT. */

            if (p == end || *p != config->varinit)
                return 0;

            if (++p == end)
                return VAR_ERR_INCOMPLETE_VARIABLE_SPEC;

            /* Try to read the variable name. If that fails, we're parsing a
               complex expression. */

            rc = varname(p, end, nameclass);
            if (rc < 0)
                return rc;
            if (rc > 0)
                {
                rc2 = (*lookup)(lookup_context, p, rc, 0, &data, &len, &buffer_size);
                if (rc2 < 0 /* != VAR_OK */)
                    return rc2;
                result->begin = data;
                result->end = data + len;
                result->buffer_size = buffer_size;
                return 1 + rc;
                }

            /* OK, we're dealing with a complex expression here. */

            rc = expression(p, end, config, nameclass, lookup, lookup_context,
                            result, current_index, rel_lookup_flag);
            if (rc > 0)
                rc++;
            return rc;
            }
        }
    }
