#include "internal.hh"

namespace varexp
    {
    using namespace internal;

    // The default configuration for the parser.

    const var_config_t var_config_default =
        {
        '$',                        // varinit
        '{',                        // startdelim
        '}',                        // enddelim
        '[',                        // startindex
        ']',                        // endindex
        '#',                        // current_index
        '\\',                       // escape
        "a-zA-Z0-9_"                // namechars
        };

    void var_expand(const char* input_buf, const size_t input_len,
                    string& result,
                    callback_t& lookup,
                    const var_config_t* config)
        {
        // Argument sanity checks

        if (input_buf == 0)
            throw invalid_argument("empty input buffer for varexp::expand()");
        if (input_len == 0)
            return;

        // If no configuration has been provided, use the default.

        if (config == 0)
            config = &var_config_default;

        // Call the parser.

        parser p(*config, lookup);
        size_t rc = p.input(input_buf, input_buf + input_len, result);
        if (rc != input_len)
            {
            input_isnt_text_nor_variable e;
            e.begin   = input_buf;
            e.current = input_buf + rc;
            e.end     = input_buf + input_len;
            throw e;
            }
        }
    }
