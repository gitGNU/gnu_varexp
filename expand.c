#include <stdlib.h>
#include <assert.h>
#include "internal.h"

#include <stdio.h>
#include <dmalloc.h>

const var_config_t var_config_default =
    {
    '$',                        /* varinit */
    '{',                        /* startdelim */
    '}',                        /* enddelim */
    '\\',                       /* escape */
    "a-zA-Z0-9_"                /* namechars */
    };

var_rc_t var_expand(const char* input_buf, size_t input_len,
                    char** result, size_t* result_len,
                    const char** error_msg,
                    var_cb_t lookup, void* lookup_context,
                    const var_config_t* config, int force_expand)
    {
    char nameclass[256];
    var_rc_t rc;
    tokenbuf output;

    /* Assert everything is as we expect it. */

    assert(input != NULL);
    assert(result != NULL);
    assert(result_len != NULL);
    assert(lookup != NULL);

    /* Expand the class description for valid variable names. */

    if (config == NULL)
        config = &var_config_default;
    rc = expand_character_class(config->namechars, nameclass);
    if (rc != VAR_OK)
        return rc;

    /* Make sure that the specials defined in the configuration do not
       appear in the character name class. */

    if (nameclass[(int)config->varinit] ||
        nameclass[(int)config->startdelim] ||
        nameclass[(int)config->enddelim] ||
        nameclass[(int)config->escape])
        return VAR_INVALID_CONFIGURATION;

    /* Call the parser. */

    output.begin = output.end = NULL;
    output.buffer_size = 0;
    rc = input(input_buf, input_buf + input_len, config, nameclass,
               lookup, lookup_context, force_expand, &output);
    if (rc != VAR_OK)
        {
        free_tokenbuf(&output);
        return rc;
        }
    *result     = (char*)output.begin;
    *result_len = output.end - output.begin;
    if (error_msg)
        *error_msg  = NULL;

    return VAR_OK;
    }
