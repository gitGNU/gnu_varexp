#include "internal.h"

int expression(const char *begin, const char *end,
               const var_config_t *config,
               const char_class_t nameclass, var_cb_t lookup,
               void *lookup_context, int force_expand,
               tokenbuf_t *result, int current_index, int *rel_lookup_flag)
    {
    const char *p = begin;
    const char *data;
    size_t len, buffer_size;
    int failed = 0;
    int rc;
    int idx = 0;
    tokenbuf_t name;
    tokenbuf_t tmp;

    /* Clear the tokenbufs to make sure we have a defined state. */

    tokenbuf_init(&name);
    tokenbuf_init(&tmp);
    tokenbuf_init(result);

    /* Expect STARTDELIM. */

    if (p == end || *p != config->startdelim)
        return 0;

    if (++p == end)
        return VAR_ERR_INCOMPLETE_VARIABLE_SPEC;

    /* Get the name of the variable to expand. The name may consist of
       an arbitrary number of VARNAMEs and VARIABLEs. */

    do {
        rc = varname(p, end, nameclass);
        if (rc < 0)
            goto error_return;
        if (rc > 0) {
            if (!tokenbuf_append(&name, p, rc)) {
                rc = VAR_ERR_OUT_OF_MEMORY;
                goto error_return;
            }
            p += rc;
        }

        rc = variable(p, end, config, nameclass, lookup, lookup_context,
                      force_expand, &tmp, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        if (rc > 0) {
            if (!tokenbuf_append(&name, tmp.begin, tmp.end - tmp.begin)) {
                rc = VAR_ERR_OUT_OF_MEMORY;
                goto error_return;
            }
            p += rc;
        }
    } while (rc > 0);

    /* We must have the complete variable name now, so make sure we
       do. */

    if (name.begin == name.end) {
        rc = VAR_ERR_INCOMPLETE_VARIABLE_SPEC;
        goto error_return;
    }

    /* If the next token is START-INDEX, read the index specification. */

    if (config->startindex && *p == config->startindex) {
        rc = num_exp(++p, end, current_index, &idx, &failed,
                     rel_lookup_flag, config, nameclass, lookup, lookup_context);
        if (rc < 0)
            goto error_return;
        if (rc == 0) {
            rc = VAR_ERR_INCOMPLETE_INDEX_SPEC;
            goto error_return;
        }
        p += rc;

        if (p == end) {
            rc = VAR_ERR_INCOMPLETE_INDEX_SPEC;
            goto error_return;
        }
        if (*p != config->endindex) {
            rc = VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC;
            goto error_return;
        }
        p++;
    }

    /* Now we have the name of the variable stored in "name". The next
       token here must either be an END-DELIM or a ':'. */

    if (p == end || (*p != config->enddelim && *p != ':')) {
        rc = VAR_ERR_INCOMPLETE_VARIABLE_SPEC;
        goto error_return;
    }
    p++;

    /* Use the lookup callback to get the variable's contents. */

    if (failed) {
        result->begin = begin - 1;
        result->end = p;
        result->buffer_size = 0;
    }
    else {
        rc = (*lookup) (lookup_context, name.begin, name.end - name.begin, idx,
                        &data, &len, &buffer_size);
        if (rc == VAR_ERR_UNDEFINED_VARIABLE) {
            /* The variable is undefined. What we'll do now depends on the
               force_expand flag. */
            if (force_expand)
                goto error_return;

            /* Initialize result to point back to the original text in
               the buffer. */
            result->begin = begin - 1;
            result->end = p;
            result->buffer_size = 0;
            failed = 1;
        }
        else if (rc < 0 /* != VAR_OK */) {
            goto error_return;
        }
        else {
            /* The preliminary result is the contents of the variable.
               This may be modified by the commands that may follow. */
            result->begin = data;
            result->end = data + len;
            result->buffer_size = buffer_size;
        }
    }

    if (p[-1] == ':') {
        /* Parse and execute commands. */

        tokenbuf_free(&tmp);
        p--;
        while (p != end && *p == ':') {
            p++;
            if (!failed)
                rc = command(p, end, config, nameclass, lookup,
                             lookup_context, force_expand, result,
                             current_index, rel_lookup_flag);
            else
                rc = command(p, end, config, nameclass, lookup,
                             lookup_context, force_expand, &tmp,
                             current_index, rel_lookup_flag);
            if (rc < 0)
                goto error_return;
            p += rc;
            if (failed)
                result->end += rc;
        }

        if (p == end || *p != config->enddelim) {
            rc = VAR_ERR_INCOMPLETE_VARIABLE_SPEC;
            goto error_return;
        }
        p++;
        if (failed)
            result->end++;
    }

    /* Exit gracefully. */

    tokenbuf_free(&name);
    tokenbuf_free(&tmp);
    return p - begin;

    /* Exit in case of an error. */

  error_return:
    tokenbuf_free(&name);
    tokenbuf_free(&tmp);
    tokenbuf_free(result);
    return rc;
}
