#include "internal.h"

var_rc_t input(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* output)
    {
    int rc;
    tokenbuf result;

    init_tokenbuf(&result);

    do
        {
        rc = text(begin, end, config->varinit, config->escape);
        if (rc > 0)
            {
            if (!append_to_tokenbuf(output, begin, rc))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            begin += rc;
            }
        else if (rc < 0)
            goto error_return;

        rc = variable(begin, end, config, nameclass, lookup, lookup_context, force_expand, &result);
        if (rc > 0)
            {
            if (!append_to_tokenbuf(output, result.begin, result.end - result.begin))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            else
                begin += rc;
            }
        else if (rc < 0)
            goto error_return;
        }
    while (rc > 0);

    if (begin != end)
        {
        rc = VAR_INPUT_ISNT_TEXT_NOR_VARIABLE;
        goto error_return;
        }

    return VAR_OK;

  error_return:
    free_tokenbuf(&result);
    return rc;
    }
