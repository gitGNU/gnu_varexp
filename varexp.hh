#ifndef LIB_VARIABLE_EXPAND_HH
#define LIB_VARIABLE_EXPAND_HH

#include <cstdlib>

namespace varexp
    {
    /* Error codes returned by the varexp library. */

    typedef enum
        {
        VAR_ERR_CALLBACK                            = -64,
        VAR_ERR_INVALID_CHAR_IN_LOOP_LIMITS         = -43,
        VAR_ERR_UNTERMINATED_LOOP_CONSTRUCT         = -42,
        VAR_ERR_DIVISION_BY_ZERO_IN_INDEX           = -41,
        VAR_ERR_UNCLOSED_BRACKET_IN_INDEX           = -40,
        VAR_ERR_INCOMPLETE_INDEX_SPEC               = -39,
        VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC          = -38,
        VAR_ERR_ARRAY_LOOKUPS_ARE_UNSUPPORTED       = -37,
        VAR_ERR_INCOMPLETE_QUOTED_PAIR              = -36,
        VAR_ERR_INVALID_ARGUMENT                    = -35,
        VAR_ERR_SUBMATCH_OUT_OF_RANGE               = -34,
        VAR_ERR_UNKNOWN_QUOTED_PAIR_IN_REPLACE      = -33,
        VAR_ERR_EMPTY_PADDING_FILL_STRING           = -32,
        VAR_ERR_MISSING_PADDING_WIDTH               = -31,
        VAR_ERR_MALFORMATTED_PADDING                = -30,
        VAR_ERR_INCORRECT_TRANSPOSE_CLASS_SPEC      = -29,
        VAR_ERR_EMPTY_TRANSPOSE_CLASS               = -28,
        VAR_ERR_TRANSPOSE_CLASSES_MISMATCH          = -27,
        VAR_ERR_MALFORMATTED_TRANSPOSE              = -26,
        VAR_ERR_OFFSET_LOGIC                        = -25,
        VAR_ERR_OFFSET_OUT_OF_BOUNDS                = -24,
        VAR_ERR_RANGE_OUT_OF_BOUNDS                 = -23,
        VAR_ERR_INVALID_OFFSET_DELIMITER            = -22,
        VAR_ERR_MISSING_START_OFFSET                = -21,
        VAR_ERR_EMPTY_SEARCH_STRING                 = -20,
        VAR_ERR_MISSING_PARAMETER_IN_COMMAND        = -19,
        VAR_ERR_INVALID_REGEX_IN_REPLACE            = -18,
        VAR_ERR_UNKNOWN_REPLACE_FLAG                = -17,
        VAR_ERR_MALFORMATTED_REPLACE                = -16,
        VAR_ERR_UNKNOWN_COMMAND_CHAR                = -14,
        VAR_ERR_INPUT_ISNT_TEXT_NOR_VARIABLE        = -13,
        VAR_ERR_UNDEFINED_VARIABLE                  = -12,
        VAR_ERR_INCOMPLETE_VARIABLE_SPEC            = -11,
        VAR_ERR_OUT_OF_MEMORY                       = -10,
        VAR_ERR_INVALID_CONFIGURATION               = -9,
        VAR_ERR_INCORRECT_CLASS_SPEC                = -8,
        VAR_ERR_INCOMPLETE_GROUPED_HEX              = -7,
        VAR_ERR_INCOMPLETE_OCTAL                    = -6,
        VAR_ERR_INVALID_OCTAL                       = -5,
        VAR_ERR_OCTAL_TOO_LARGE                     = -4,
        VAR_ERR_INVALID_HEX                         = -3,
        VAR_ERR_INCOMPLETE_HEX                      = -2,
        VAR_ERR_INCOMPLETE_NAMED_CHARACTER          = -1,
        VAR_OK                                      = 0
        }
    var_rc_t;

    /* Expand quoted pairs to their binary representation. */

    var_rc_t var_unescape(const char* src, size_t len, char* dst, int unescape_all);

    /* Prototype for the lookup callback used in var_expand(). */

    typedef var_rc_t (*var_cb_t)(void* context,
                                 const char* varname, size_t name_len, int idx,
                                 const char* *data, size_t* data_len,
                                 size_t* buffer_size);

    /* Configure the var_expand() parser's tokens. */

    typedef struct
        {
        char varinit;        /* '$' */
        char startdelim;     /* '{' */
        char enddelim;       /* '}' */
        char startindex;     /* '[' */
        char endindex;       /* ']' */
        char current_index;  /* '#' */
        char escape;         /* '\' */
        char* namechars;     /* 'a-zA-Z0-9_' */
        }
    var_config_t;
    extern const var_config_t var_config_default;

    /* Expand variable expressions in a text buffer. */

    var_rc_t var_expand(const char* input, size_t input_len,
                        char* *result, size_t* result_len,
                        var_cb_t lookup, void* lookup_context,
                        const var_config_t* config);

    /* Map an error code to a text message. */

    const char* var_strerror(int rc);
    }

#endif /* !defined(LIB_VARIABLE_EXPAND_HH) */
