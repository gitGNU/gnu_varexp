#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "internal.h"

#include <stdio.h>
#include <dmalloc.h>

int variable(const char* begin, const char* end, const var_config_t* config,
             const char nameclass[256], var_cb_t lookup, void* lookup_context,
             int force_expand, tokenbuf* result)
    {
    const char* p = begin;
    const char* data;
    size_t len, buffer_size;
    int rc, rc2;

    /* Clear the result tokenbuf to make sure we're in a defined
       state. */

    init_tokenbuf(result);

    /* Expect VARINIT. */

    if (p == end || *p != config->varinit)
        return 0;

    if (++p == end)
        return VAR_INCOMPLETE_VARIABLE_SPEC;

    /* Try to read the variable name. If that fails, we're parsing a
       complex expression. */

    rc = varname(p, end, nameclass);
    if (rc < 0)
        return rc;
    else if (rc > 0)
        {
        rc2 = (*lookup)(lookup_context, p, rc, &data, &len, &buffer_size);
        if (rc2 < 0)
            return rc2;
        else if (rc2 == 0)
            {
            if (force_expand)
                return VAR_UNDEFINED_VARIABLE;
            else
                {
                result->begin       = begin;
                result->end         = begin + 1 + rc;
                result->buffer_size = 0;
                return 1 + rc;
                }
            }
        else
            {
            result->begin       = data;
            result->end         = data + len;
            result->buffer_size = buffer_size;
            return 1 + rc;
            }
        }

    /* OK, we're dealing with a complex expression here. */

    rc = expression(p, end, config, nameclass, lookup, lookup_context, force_expand, result);
    if (rc > 0)
        ++rc;
    return rc;
    }
