#include "internal.hh"

namespace varexp
    {
    /* The var_strerror() routine will map a var_rc_t into a text message. */

    const char* var_strerror(int rc)
        {
        static char* var_errors[] =
            {
            "OK",                                                                   /* VAR_OK = 0 */
            "incomplete named character",                                           /* VAR_ERR_INCOMPLETE_NAMED_CHARACTER = -1 */
            "incomplete hex",                                                       /* VAR_ERR_INCOMPLETE_HEX = -2 */
            "invalid hex",                                                          /* VAR_ERR_INVALID_HEX = -3 */
            "octal too large",                                                      /* VAR_ERR_OCTAL_TOO_LARGE = -4 */
            "invalid octal",                                                        /* VAR_ERR_INVALID_OCTAL = -5 */
            "incomplete octal",                                                     /* VAR_ERR_INCOMPLETE_OCTAL = -6 */
            "incomplete grouped hex",                                               /* VAR_ERR_INCOMPLETE_GROUPED_HEX = -7 */
            "incorrect character class specification",                              /* VAR_ERR_INCORRECT_CLASS_SPEC = -8 */
            "var_expand() configuration is inconsistent",                           /* VAR_ERR_INVALID_CONFIGURATION = -9 */
            "out of memory",                                                        /* VAR_ERR_OUT_OF_MEMORY = -10 */
            "incomplete variable",                                                  /* VAR_ERR_INCOMPLETE_VARIABLE_SPEC = -11 */
            "undefined variable",                                                   /* VAR_ERR_UNDEFINED_VARIABLE = -12 */
            "input is neither text nor variable",                                   /* VAR_ERR_INPUT_ISNT_TEXT_NOR_VARIABLE = -13 */
            "unknown command in variable",                                          /* VAR_ERR_UNKNOWN_COMMAND_CHAR = -14 */
            "unknown error",                                                        /* -15 is not used */
            "malformated search and replace operation",                             /* VAR_ERR_MALFORMATTED_REPLACE = -16 */
            "unknown flag specified in search and replace operation",               /* VAR_ERR_UNKNOWN_REPLACE_FLAG = -17 */
            "invalid regular expression in search and replace operation",           /* VAR_ERR_INVALID_REGEX_IN_REPLACE = -18 */
            "missing parameter in command",                                         /* VAR_ERR_MISSING_PARAMETER_IN_COMMAND = -19 */
            "empty search string in search and replace operation",                  /* VAR_ERR_EMPTY_SEARCH_STRING = -20 */
            "start offset missing in cut operation",                                /* VAR_ERR_MISSING_START_OFFSET = -21 */
            "offsets in cut operation delimited by unknown character",              /* VAR_ERR_INVALID_OFFSET_DELIMITER = -22 */
            "range in cut operation is out of bounds",                              /* VAR_ERR_RANGE_OUT_OF_BOUNDS = -23 */
            "offset in cut operation is out of bounds",                             /* VAR_ERR_OFFSET_OUT_OF_BOUNDS = -24 */
            "logic error in cut operation",                                         /* VAR_ERR_OFFSET_LOGIC = -25 */
            "malformatted transpose operation",                                     /* VAR_ERR_MALFORMATTED_TRANSPOSE = -26 */
            "source class does not match destination class in transpose operation", /* VAR_ERR_TRANSPOSE_CLASSES_MISMATCH = -27 */
            "empty character class in transpose operation",                         /* VAR_ERR_EMPTY_TRANSPOSE_CLASS = -28 */
            "incorrect character class in transpose operation",                     /* VAR_ERR_INCORRECT_TRANSPOSE_CLASS_SPEC = -29 */
            "malformatted padding operation",                                       /* VAR_ERR_MALFORMATTED_PADDING = -30 */
            "width parameter missing in padding operation",                         /* VAR_ERR_MISSING_PADDING_WIDTH = -31 */
            "fill string missing in padding operation",                             /* VAR_ERR_EMPTY_PADDING_FILL_STRING = -32 */
            "unknown quoted pair in search and replace operation",                  /* VAR_ERR_UNKNOWN_QUOTED_PAIR_IN_REPLACE = -33 */
            "submatch referred to in replace string does not exist in search string", /* VAR_ERR_SUBMATCH_OUT_OF_RANGE = -34 */
            "invalid argument",                                                     /* VAR_ERR_INVALID_ARGUMENT = -35 */
            "incomplete quoted pair",                                               /* VAR_ERR_INCOMPLETE_QUOTED_PAIR = -36 */
            "lookup function does not support variable arrays",                     /* VAR_ERR_ARRAY_LOOKUPS_ARE_UNSUPPORTED = -37 */
            "index specification of array variable contains an invalid character",  /* VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC = -38 */
            "index specification of array variable is incomplete",                  /* VAR_ERR_INCOMPLETE_INDEX_SPEC = -39 */
            "bracket expression in array variable's index is not closed",           /* VAR_ERR_UNCLOSED_BRACKET_IN_INDEX = -40 */
            "division by zero error in index specification",                        /* VAR_ERR_DIVISION_BY_ZERO_IN_INDEX = -41 */
            "unterminated loop construct",                                          /* VAR_ERR_UNTERMINATED_LOOP_CONSTRUCT = -42 */
            "invalid character in loop limits"                                      /* VAR_ERR_INVALID_CHAR_IN_LOOP_LIMITS = -43 */
            };

        rc = 0 - rc;
        if (rc < 0 || rc >= (int)(sizeof(var_errors) / sizeof(char*)))
            return "unknown error";

        return var_errors[rc];
        }
    }
