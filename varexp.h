#ifndef LIB_VARIABLE_EXPAND_H
#define LIB_VARIABLE_EXPAND_H

#include <stdlib.h>

/* Error codes returned by the varexp library. */

typedef enum
    {
    VAR_INCORRECT_TRANSPOSE_CLASS_SPEC = -29,
    VAR_EMPTY_TRANSPOSE_CLASS          = -28,
    VAR_TRANSPOSE_CLASSES_MISMATCH     = -27,
    VAR_MALFORMATTED_TRANSPOSE         = -26,
    VAR_OFFSET_LOGIC_ERROR             = -25,
    VAR_OFFSET_OUT_OF_BOUNDS           = -24,
    VAR_RANGE_OUT_OF_BOUNDS            = -23,
    VAR_INVALID_OFFSET_DELIMITER       = -22,
    VAR_MISSING_START_OFFSET           = -21,
    VAR_EMPTY_SEARCH_STRING            = -20,
    VAR_MISSING_PARAMETER_IN_COMMAND   = -19,
    VAR_INVALID_REGEX_IN_REPLACE       = -18,
    VAR_UNKNOWN_REPLACE_FLAG           = -17,
    VAR_MALFORMATTED_REPLACE           = -16,
    VAR_UNKNOWN_COMMAND_CHAR           = -14,
    VAR_INPUT_ISNT_TEXT_NOR_VARIABLE   = -13,
    VAR_UNDEFINED_VARIABLE             = -12,
    VAR_INCOMPLETE_VARIABLE_SPEC       = -11,
    VAR_OUT_OF_MEMORY                  = -10,
    VAR_INVALID_CONFIGURATION          = -9,
    VAR_INCORRECT_CLASS_SPEC           = -8,
    VAR_INCOMPLETE_GROUPED_HEX         = -7,
    VAR_INCOMPLETE_OCTAL               = -6,
    VAR_INVALID_OCTAL                  = -5,
    VAR_OCTAL_TOO_LARGE                = -4,
    VAR_INVALID_HEX                    = -3,
    VAR_INCOMPLETE_HEX                 = -2,
    VAR_INCOMPLETE_NAMED_CHARACTER     = -1,
    VAR_INCOMPLETE_QUOTED_PAIR         = -1,
    VAR_OK                             = 0
    }
var_rc_t;

/*
   Expand the following named characters to their binary
   representation:

       \t          tab
       \n          newline
       \r          return
       \033        octal char
       \x1B        hex char
       \x{263a}    wide hex char

  Any other character quoted by a backslash is copied verbatim.
*/

var_rc_t expand_named_characters(const char* src, size_t len, char* dst);

/*
   The callback will be called by variable_expand(), providing the
   following parameterns:

        context         - passed through from variable_expand()'s
                          parameters
        varname         - pointer to the name of the variable to
                          expand
        name_len        - length of the string starting at varname
        data            - location, where the callback should store
                          the pointer to the contents of the looked-up
                          variable
        data_len        - location, where the callback should store
                          the length of the data
        malloced_buffer - location, where the callback should store
                          either TRUE or FALSE, telling the framework
                          whether the buffer must be free(3)ed.

   The return code is interpreted as follows:
       >0               - OK
        0               - undefined variable
       <0 - error
*/

typedef int (*var_cb_t)(void* context,
                        const char* varname, size_t name_len,
                        const char** data, size_t* data_len, size_t* buffer_size);

/*
   This structure configures the parser's specials. I think, the fields
   are pretty self-explanatory. The only one worth mentioning is
   force_expand, which is a boolean. If set to TRUE, variable_expand()
   will fail with an error if the lookup callback returns "undefined
   variable". If set to FALSE, variable_expand() will copy the
   expression that failed verbatimly to the output so that another pass
   may expand it.

   The comments after each field show the default configuration.
*/

typedef struct
    {
    char  varinit;               /* '$' */
    char  startdelim;            /* '{' */
    char  enddelim;              /* '}' */
    char  escape;                /* '\' */
    char* namechars;             /* 'a-zA-Z0-9_' */
    }
var_config_t;
extern const var_config_t var_config_default;

/*
   variable_expand() will parse the contents of input for variable
   expressions and expand them using the provided lookup callback. The
   pointer to the resulting buffer is stored in result, its length in
   result_len. The buffer is always terminated by a '\0' byte, which is
   not included in the result_len count. The buffer must be free(3)ed
   by the caller.
*/

var_rc_t var_expand(const char* input, size_t input_len,
                    char** result, size_t* result_len,
                    var_cb_t lookup, void* lookup_context,
                    const var_config_t* config, int force_expand);

#endif /* !defined(LIB_VARIABLE_EXPAND_H) */
