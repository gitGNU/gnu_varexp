#include "internal.hh"

namespace varexp
    {
    using namespace internal;

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

    void var_expand(const char* input_buf, size_t input_len,
                        char** result, size_t* result_len,
                        var_cb_t lookup, void* lookup_context,
                        const var_config_t* config)
        {
        char_class_t nameclass;
        tokenbuf_t output;

        /* Argument sanity checks */
        if (input_buf == NULL || input_len == 0 ||
            result == NULL ||
            lookup == NULL)
            throw invalid_argument();

        /* Optionally use default configuration */
        if (config == NULL)
            config = &var_config_default;

        /* Set the result pointer to the begining of the input buffer so
           that it is correctly initialized in case we fail with an error. */
        *result = (char*)input_buf;

        /* Expand the class description for valid variable names. */
        expand_character_class(config->namechars, nameclass);

        /* Make sure that the specials defined in the configuration do not
           appear in the character name class. */
        if (nameclass[(int)config->varinit] ||
            nameclass[(int)config->startdelim] ||
            nameclass[(int)config->enddelim] ||
            nameclass[(int)config->escape])
            throw invalid_configuration();

        /* Call the parser. */

        input(input_buf, input_buf + input_len, config, nameclass,
              lookup, lookup_context, &output, 0, 0, NULL);

        // Always NUL-terminate output for convinience.

        output.append("\0", 1);
        --output.end;

        // Provide results.

        *result = (char*)output.begin;
        if (result_len != NULL)
            *result_len = output.end - output.begin;
        output.buffer_size = 0;
        }
    }
