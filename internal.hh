#ifndef LIB_VARIABLE_EXPAND_INTERNAL_HH
#define LIB_VARIABLE_EXPAND_INTERNAL_HH

#include <cassert>
#include <cctype>
#include <string>
#include <memory>
#include <cstdio>
#include <cerrno>
#include "varexp.hh"

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

        size_t command(const char *begin, const char *end,
                       const var_config_t *config, const char_class_t nameclass,
                       var_cb_t lookup, void *lookup_context,
                       tokenbuf_t *data, int current_index, int *rel_lookup_flag);

        void cut_out_offset(tokenbuf_t *data, tokenbuf_t *number1,
                            tokenbuf_t *number2, int isrange);

        size_t expression(const char *begin, const char *end,
                          const var_config_t *config,
                          const char_class_t nameclass, var_cb_t lookup,
                          void *lookup_context,
                          tokenbuf_t *result, int current_index, int *rel_lookup_flag);

        size_t input(const char *begin, const char *end,
                     const var_config_t *config,
                     const char_class_t nameclass, var_cb_t lookup,
                     void *lookup_context,
                     tokenbuf_t *output, int current_index,
                     size_t recursion_level, int *rel_lookup_flag);

        size_t loop_limits(const char *begin, const char *end,
                           const var_config_t *config,
                           const char_class_t nameclass,
                           var_cb_t lookup, void* lookup_context,
                           int* start, int* step, int* stop, int* open_end);

        size_t num_exp(const char *begin, const char *end, int current_index,
                       int *result, int *rel_lookup_flag,
                       const var_config_t *config,
                       const char_class_t nameclass,
                       var_cb_t lookup, void *lookup_context);

        void padding(tokenbuf_t *data, tokenbuf_t *widthstr, tokenbuf_t *fill,
                     char position);

        void search_and_replace(tokenbuf_t *data, tokenbuf_t *search,
                                tokenbuf_t *replace, tokenbuf_t *flags);

        size_t number(const char *begin, const char *end);

        size_t exptext(const char *begin, const char *end,
                       const var_config_t *config);

        size_t exptext_or_variable(const char *begin, const char *end,
                                   const var_config_t *config,
                                   const char_class_t nameclass, var_cb_t lookup,
                                   void *lookup_context,
                                   tokenbuf_t *result, int current_index,
                                   int *rel_lookup_flag);

        size_t substext_or_variable(const char *begin, const char *end,
                                    const var_config_t *config,
                                    const char_class_t nameclass, var_cb_t lookup,
                                    void *lookup_context,
                                    tokenbuf_t *result, int current_index,
                                    int *rel_lookup_flag);

        void transpose(tokenbuf_t *data, tokenbuf_t *search,
                       tokenbuf_t *replace);

        void expand_character_class(const char *desc, char_class_t char_class);

        size_t variable(const char *begin, const char *end,
                        const var_config_t *config, const char_class_t nameclass,
                        var_cb_t lookup, void *lookup_context,
                        tokenbuf_t *result, int current_index,
                        int *rel_lookup_flag);

        size_t varname(const char *begin, const char *end,
                       const char_class_t nameclass);

        struct wrapper_context
            {
            var_cb_t lookup;
            void    *context;
            int     *rel_lookup_flag;
            };

        int lookup_wrapper(void* context,
                           const char* name, size_t name_len, int idx,
                           const char** data, size_t* data_len,
                           size_t* buffer_size);

        size_t text(const char *begin, const char *end, char varinit,
                    char startindex, char endindex, char escape);

        }
    }

#endif /* !defined(LIB_VARIABLE_EXPAND_INTERNAL_HH) */
