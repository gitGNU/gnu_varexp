#include "internal.hh"

namespace varexp
    {
    using namespace internal;

    // The default configuration for the parser.

    const config_t config_default =
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

    void expand(const string& buffer, string& result, callback_t& lookup, const config_t* config)
        {
        // Argument sanity checks

        if (buffer.empty())
            return;

        // If no configuration has been provided, use the default.

        if (config == 0)
            config = &config_default;

        // Call the parser.

        parser p(*config, lookup);
        size_t rc = p.input(buffer.data(), buffer.data() + buffer.size(), result);
        if (rc != buffer.size())
            {
            input_isnt_text_nor_variable e;
            e.current_position = rc;
            throw e;
            }
        }
    }
