#ifndef LIB_VARIABLE_EXPAND_HH
#define LIB_VARIABLE_EXPAND_HH

#include <cstdlib>
#include <stdexcept>
#include <string>

namespace varexp
    {
    // Exceptions thrown by the varexp library.

    struct error : public std::runtime_error
        {
        error(const std::string& _what)
                : std::runtime_error(_what), current_position(0)
            {
            }
        virtual ~error() throw() = 0;
        size_t current_position;
        };

#define define_exception(name, msg) \
    struct name : public error      \
        {                           \
        name() : error(msg) { }     \
        ~name() throw() { }         \
        }

    define_exception(incomplete_named_character,     "incomplete named character");
    define_exception(incomplete_hex,                 "incomplete hex");
    define_exception(invalid_hex,                    "invalid hex");
    define_exception(octal_too_large,                "octal too large");
    define_exception(invalid_octal,                  "invalid octal");
    define_exception(incomplete_octal,               "incomplete octal");
    define_exception(incomplete_grouped_hex,         "incomplete grouped hex");
    define_exception(incorrect_class_spec,           "incorrect character class specification");
    define_exception(invalid_configuration,          "varex::expand()'s configuration is inconsistent");
    define_exception(out_of_memory,                  "out of memory");
    define_exception(incomplete_variable_spec,       "incomplete variable");
    define_exception(undefined_variable,             "undefined variable");
    define_exception(input_isnt_text_nor_variable,   "input is neither text nor variable");
    define_exception(unknown_command_char,           "unknown command in variable");
    define_exception(malformatted_replace,           "malformated search and replace operation");
    define_exception(unknown_replace_flag,           "unknown flag specified in search and replace operation");
    define_exception(invalid_regex_in_replace,       "invalid regular expression in search and replace operation");
    define_exception(missing_parameter_in_command,   "missing parameter in command");
    define_exception(empty_search_string,            "empty search string in search and replace operation");
    define_exception(missing_start_offset,           "start offset missing in cut operation");
    define_exception(invalid_offset_delimiter,       "offsets in cut operation delimited by unknown character");
    define_exception(range_out_of_bounds,            "range in cut operation is out of bounds");
    define_exception(offset_out_of_bounds,           "offset in cut operation is out of bounds");
    define_exception(offset_logic,                   "logic error in cut operation");
    define_exception(malformatted_transpose,         "malformatted transpose operation");
    define_exception(transpose_classes_mismatch,     "source class does not match destination class in transpose operation");
    define_exception(empty_transpose_class,          "empty character class in transpose operation");
    define_exception(incorrect_transpose_class_spec, "incorrect character class in transpose operation");
    define_exception(malformatted_padding,           "malformatted padding operation");
    define_exception(missing_padding_width,          "width parameter missing in padding operation");
    define_exception(empty_padding_fill_string,      "fill string missing in padding operation");
    define_exception(unknown_quoted_pair_in_replace, "unknown quoted pair in search and replace operation");
    define_exception(submatch_out_of_range,          "submatch referred to in replace string does not exist in search string");
    define_exception(incomplete_quoted_pair,         "incomplete quoted pair");
    define_exception(array_lookups_are_unsupported,  "lookup function does not support variable arrays");
    define_exception(invalid_char_in_index_spec,     "index specification of array variable contains an invalid character");
    define_exception(incomplete_index_spec,          "index specification of array variable is incomplete");
    define_exception(unclosed_bracket_in_index,      "bracket expression in array variable's index is not closed");
    define_exception(division_by_zero_in_index,      "division by zero error in index specification");
    define_exception(unterminated_loop_construct,    "unterminated loop construct");
    define_exception(invalid_char_in_loop_limits,    "invalid character in loop limits");
#undef define_exception

    extern "C" {

    // Expand quoted pairs to their binary representation.

    void unescape(const char* src, size_t len, char* dst, int unescape_all);

    // Prototype for the lookup callback used in expand().

    struct callback_t
        {
        virtual void operator()(const std::string& name, std::string& data) = 0;
        virtual void operator()(const std::string& name, int idx, std::string& data) = 0;
        };

    // Configure the expand() parser's tokens.

    struct config_t
        {
        char  varinit;        // '$'
        char  startdelim;     // '{'
        char  enddelim;       // '}'
        char  startindex;     // '['
        char  endindex;       // ']'
        char  current_index;  // '#'
        char  escape;         // '\'
        char* namechars;      // 'a-zA-Z0-9_'
        };
    extern const config_t config_default;

    // Expand variable expressions in a text buffer.

    void expand(const std::string& input, std::string& result, callback_t& lookup, const config_t* config = 0);

    }
    }

#endif // !defined(LIB_VARIABLE_EXPAND_HH)
