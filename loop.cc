#include "internal.h"

var_rc_t lookup_wrapper(void* context,
                        const char* name, size_t name_len, int idx,
                        const char** data, size_t* data_len,
                        size_t* buffer_size)
    {
    static char buf[1];
    struct wrapper_context* wcon = static_cast<wrapper_context*>(context);
    var_rc_t rc;

    rc = (*wcon->lookup)(wcon->context, name, name_len,
                         idx, data, data_len, buffer_size);
    if (rc == VAR_ERR_UNDEFINED_VARIABLE)
        {
        (*wcon->rel_lookup_flag)--;
        *data = buf;
        *data_len = 0;
        *buffer_size = 0;
        return VAR_OK;
        }
    return rc;
    }

var_rc_t loop_limits(const char* begin, const char* end,
                     const var_config_t* config,
                     const char_class_t nameclass,
                     var_cb_t lookup, void* lookup_context,
                     int* start, int* step, int* stop, int* open_end)
    {
    const char* p = begin;
    int rc;
    int dummy;

    if (begin == end)
        return VAR_OK;

    if (*p != config->startdelim)
        return VAR_OK;
    else
        ++p;

    /* Read start value for the loop. */

    rc = num_exp(p, end, 0, start, &dummy,
                 config, nameclass, lookup, lookup_context);
    if (rc == VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC)
        *start = 0;          /* use default */
    else if (rc < 0)
        return var_rc_t(rc);
    else
        p += rc;

    if (*p != ',')
        return VAR_ERR_INVALID_CHAR_IN_LOOP_LIMITS;
    else
        ++p;

    /* Read step value for the loop. */

    rc = num_exp(p, end, 0, step, &dummy,
                 config, nameclass, lookup, lookup_context);
    if (rc == VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC)
        *step = 1;          /* use default */
    else if (rc < 0)
        return var_rc_t(rc);
    else
        p += rc;

    if (*p != ',')
        {
        if (*p != config->enddelim)
            return VAR_ERR_INVALID_CHAR_IN_LOOP_LIMITS;
        else
            {
            ++p;
            *stop = *step;
            *step = 1;
            if (rc > 0)
                *open_end = 0;
            else
                *open_end = 1;
            return var_rc_t(p - begin);
            }
        }
    else
        ++p;

    /* Read stop value for the loop. */

    rc = num_exp(p, end, 0, stop, &dummy,
                 config, nameclass, lookup, lookup_context);
    if (rc == VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC)
        {
        *stop = 0;          /* use default */
        *open_end = 1;
        }
    else if (rc < 0)
        return var_rc_t(rc);
    else
        {
        *open_end = 0;
        p += rc;
        }

    if (*p != config->enddelim)
        return VAR_ERR_INVALID_CHAR_IN_LOOP_LIMITS;

    return var_rc_t(++p - begin);
    }
