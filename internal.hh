#ifndef LIB_VARIABLE_EXPAND_INTERNAL_HH
#define LIB_VARIABLE_EXPAND_INTERNAL_HH

#include <cassert>
#include <cctype>
#include <memory>
#include <cstdio>
#include <cerrno>
#include "varexp.hh"
using namespace std;

extern "C" {

namespace varexp
    {
    namespace internal
        {
        // Turn character class descriptions into a lookup-array.

        typedef char char_class_t[256]; /* 256 == 2 ^ sizeof(unsigned char)*8 */

        // The tokenbuf_t class.

        class tokenbuf_t
            {
          public:
            const char *begin;
            const char *end;
            size_t buffer_size;

            tokenbuf_t();
            ~tokenbuf_t();
            void clear();
            void force_copy();
            void assign(const char* data, size_t len);
            void append(const char* data, size_t len);
            void shallow_move(tokenbuf_t* src);
            unsigned int toint();

          private:
            static const size_t INITIAL_BUFSIZE;
            };

        class parser
            {
          protected:
            friend void varexp::var_expand(const char* input_buf, const size_t input_len,
                                           std::string& result, callback_t& lookup,
                                           const var_config_t* config);

            parser(const var_config_t& _config, callback_t& _lookup);
            size_t input(const char* begin, const char* end, string& result);

          private:
            size_t command(const char *begin, const char *end, std::string& result);
            size_t expression(const char *begin, const char *end, std::string& result);
            size_t loop_limits(const char *begin, const char *end, int& start, int& step, int& stop, bool& open_end);
            size_t num_exp_read_operand(const char* begin, const char* end, int& result);
            size_t num_exp(const char *begin, const char *end, int& result);
            size_t number(const char *begin, const char *end, unsigned int& num);
            size_t exptext(const char *begin, const char *end);
            size_t exptext_or_variable(const char *begin, const char *end, std::string& result);
            size_t substext_or_variable(const char *begin, const char *end, std::string& result);
            size_t variable(const char *begin, const char *end, std::string& result);
            size_t varname(const char *begin, const char *end);
            size_t text(const char *begin, const char *end);
            size_t substext(const char* begin, const char* end);

            const var_config_t& config;
            char_class_t        nameclass;
            callback_t*         lookup;
            int                 current_index;
            int                 rel_lookup_count;
            };

        void cut_out_offset(std::string& data, unsigned int num1, unsigned int num2, bool is_range);

        void padding(std::string& data, unsigned int width, const std::string& fillstring, char position);

        void search_and_replace(std::string& data, const std::string& search,
                                const std::string& replace, const std::string& flags);

        void transpose(std::string& data, const std::string& srcclass, const std::string& dstclass);

        void expand_character_class(const char *desc, char_class_t char_class);

        size_t ascii_to_uint(const char* begin, const char* end, unsigned int& result);
        }
    }

}
#endif /* !defined(LIB_VARIABLE_EXPAND_INTERNAL_HH) */
