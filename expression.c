#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "internal.h"

#include <stdio.h>

int expression(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* result)
    {
    const char* p = begin;
    const char* data;
    size_t len, buffer_size;
    int failed = 0;
    int rc;
    tokenbuf name;
    tokenbuf tmp;

    /* Clear the tokenbufs to make sure we have a defined state. */

    init_tokenbuf(&name);
    init_tokenbuf(&tmp);
    init_tokenbuf(result);

    /* Expect STARTDELIM. */

    if (p == end || *p != config->startdelim)
        return 0;

    if (++p == end)
        return VAR_INCOMPLETE_VARIABLE_SPEC;

    /* Get the name of the variable to expand. The name may consist of
       an arbitrary number of VARNAMEs and VARIABLEs. */

    do
        {
        printf("expression(): Name of the variable to expand is '%s'\n", name.begin);

        rc = varname(p, end, nameclass);
        if (rc < 0)
            goto error_return;
        else if (rc > 0)
            {
            if (!append_to_tokenbuf(&name, p, rc))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            else
                p += rc;
            }

        rc = variable(p, end, config, nameclass, lookup, lookup_context, force_expand, &tmp);
        if (rc < 0)
            goto error_return;
        else if (rc > 0)
            {
            if (!append_to_tokenbuf(&name, tmp.begin, tmp.end - tmp.begin))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            else
                p += rc;
            }
        }
    while (rc > 0);

    /* We must have the complete variable name now, so make sure we
       do. */

    if (name.begin == name.end)
        {
        rc = VAR_INCOMPLETE_VARIABLE_SPEC;
        goto error_return;
        }

    /* Now we have the name of the variable stored in "name". We
       expect an ENDDELIM here. */

    if (p == end || (*p != config->enddelim && *p != ':'))
        {
        rc = VAR_INCOMPLETE_VARIABLE_SPEC;
        goto error_return;
        }
    else
        ++p;

    /* Use the lookup callback to get the variable's contents. */

    rc = (*lookup)(lookup_context, name.begin, name.end - name.begin, &data, &len, &buffer_size);
    if (rc < 0)
        goto error_return;
    else if (rc == 0)
        {
        /* The variable is undefined. What we'll do now depends on the
           force_expand flag. */

        if (force_expand)
            {
            rc = VAR_UNDEFINED_VARIABLE;
            goto error_return;
            }
        else
            {
            /* Initialize result to point back to the original text in
               the buffer. */

            result->begin       = begin-1;
            result->end         = p;
            result->buffer_size = 0;
            failed              = 1;
            }
        }
    else
        {
        /* The preliminary result is the contents of the variable.
           This may be modified by the commands that may follow. */

        result->begin       = data;
        result->end         = data + len;
        result->buffer_size = buffer_size;
        }

    if (p[-1] == ':')
        {
        /* Parse and execute commands. */

        free_tokenbuf(&tmp);
        --p;
        while (p != end && *p == ':')
            {
            ++p;
            if (!failed)
                rc = command(p, end, config, nameclass, lookup, lookup_context, force_expand, result);
            else
                rc = command(p, end, config, nameclass, lookup, lookup_context, force_expand, &tmp);
            if (rc < 0)
                goto error_return;
            p += rc;
            if (failed)
                result->end += rc;
            }

        if (p == end || *p != config->enddelim)
            {
            rc = VAR_INCOMPLETE_VARIABLE_SPEC;
            goto error_return;
            }
        ++p;
        if (failed)
            result->end += rc;
        }

    /* Exit gracefully. */

    free_tokenbuf(&name);
    free_tokenbuf(&tmp);
    return p - begin;

    /* Exit in case of an error. */

  error_return:
    free_tokenbuf(&name);
    free_tokenbuf(&tmp);
    free_tokenbuf(result);
    return rc;
    }
