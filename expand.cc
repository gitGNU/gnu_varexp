#include "internal.hh"

/* The default configuration for the parser. */

const var_config_t var_config_default =
    {
    '$',                        /* varinit */
    '{',                        /* startdelim */
    '}',                        /* enddelim */
    '[',                        /* startindex */
    ']',                        /* endindex */
    '#',                        /* current_index */
    '\\',                       /* escape */
    "a-zA-Z0-9_"                /* namechars */
    };

var_rc_t var_expand(const char* input_buf, size_t input_len,
                    char** result, size_t* result_len,
                    var_cb_t lookup, void* lookup_context,
                    const var_config_t* config)
    {
    char_class_t nameclass;
    var_rc_t rc;
    tokenbuf_t output;

    /* Argument sanity checks */
    if (input_buf == NULL || input_len == 0 ||
        result == NULL ||
        lookup == NULL)
        return VAR_ERR_INVALID_ARGUMENT;

    /* Optionally use default configuration */
    if (config == NULL)
        config = &var_config_default;

    /* Set the result pointer to the begining of the input buffer so
       that it is correctly initialized in case we fail with an error. */
    *result = (char*)input_buf;

    /* Expand the class description for valid variable names. */
    if ((rc = expand_character_class(config->namechars, nameclass)) != VAR_OK)
        return rc;

    /* Make sure that the specials defined in the configuration do not
       appear in the character name class. */
    if (nameclass[(int)config->varinit] ||
        nameclass[(int)config->startdelim] ||
        nameclass[(int)config->enddelim] ||
        nameclass[(int)config->escape])
        return VAR_ERR_INVALID_CONFIGURATION;

    /* Call the parser. */
    tokenbuf_init(&output);
    rc = input(input_buf, input_buf + input_len, config, nameclass,
               lookup, lookup_context, &output, 0, 0, NULL);

    /* Post-process output */
    if (rc >= 0)
        {
        /* always NUL-terminate output for convinience reasons */
        if (!tokenbuf_append(&output, "\0", 1))
            {
            tokenbuf_free(&output);
            return VAR_ERR_OUT_OF_MEMORY;
            }
        output.end--;

        /* Provide results */
        *result = (char*)output.begin;
        if (result_len != NULL)
            *result_len = output.end - output.begin;

        /* canonify all positive answers */
        rc = VAR_OK;
        }
    else
        {
        /* Provide error results */
        *result = (char*)input_buf;
        if (result_len != NULL)
            *result_len = output.end - output.begin;
        }

    return rc;
    }
