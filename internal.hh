#ifndef INTERNAL_H
#define INTERNAL_H

#include <cassert>
#include <cctype>
#include <string>
#include <cstdio>
#include <cerrno>
#include "varexp.hh"

/* Turn character class descriptions into a lookup-array. */

typedef char char_class_t[256]; /* 256 == 2 ^ sizeof(unsigned char)*8 */

/*
   The tokenbuf structure is used by the parser routines. If
   buffer_size is >0, it means that the buffer has been allocated by
   malloc(3) and must be free(3)ed when not used anymore.
*/

typedef struct
    {
    const char *begin;
    const char *end;
    size_t buffer_size;
    }
tokenbuf_t;

void tokenbuf_init(tokenbuf_t *buf);
void tokenbuf_move(tokenbuf_t *src, tokenbuf_t *dst);
int tokenbuf_assign(tokenbuf_t *buf, const char *data, size_t len);
int tokenbuf_append(tokenbuf_t *output, const char *data, size_t len);
void tokenbuf_free(tokenbuf_t *buf);
size_t tokenbuf_toint(tokenbuf_t *number);

#endif /* !defined(INTERNAL_H) */

int command(const char *begin, const char *end,
            const var_config_t *config, const char_class_t nameclass,
            var_cb_t lookup, void *lookup_context,
            tokenbuf_t *data, int current_index, int *rel_lookup_flag);

int cut_out_offset(tokenbuf_t *data, tokenbuf_t *number1,
                   tokenbuf_t *number2, int isrange);

var_rc_t var_unescape(const char *src, size_t len, char *dst,
                      int unescape_all);

var_rc_t var_expand(const char *input_buf, size_t input_len,
                    char **result, size_t *result_len,
                    var_cb_t lookup, void *lookup_context,
                    const var_config_t *config);

int expression(const char *begin, const char *end,
               const var_config_t *config,
               const char_class_t nameclass, var_cb_t lookup,
               void *lookup_context,
               tokenbuf_t *result, int current_index, int *rel_lookup_flag);

var_rc_t input(const char *begin, const char *end,
               const var_config_t *config,
               const char_class_t nameclass, var_cb_t lookup,
               void *lookup_context,
               tokenbuf_t *output, int current_index,
               size_t recursion_level, int *rel_lookup_flag);

var_rc_t loop_limits(const char *begin, const char *end,
                     const var_config_t *config,
                     const char_class_t nameclass,
                     var_cb_t lookup, void* lookup_context,
                     int* start, int* step, int* stop, int* open_end);

int num_exp(const char *begin, const char *end, int current_index,
            int *result, int *rel_lookup_flag,
            const var_config_t *config,
            const char_class_t nameclass,
            var_cb_t lookup, void *lookup_context);

int padding(tokenbuf_t *data, tokenbuf_t *widthstr, tokenbuf_t *fill,
            char position);

int search_and_replace(tokenbuf_t *data, tokenbuf_t *search,
                       tokenbuf_t *replace, tokenbuf_t *flags);

int number(const char *begin, const char *end);

int exptext(const char *begin, const char *end,
            const var_config_t *config);

const char* var_strerror(var_rc_t rc);

int exptext_or_variable(const char *begin, const char *end,
                        const var_config_t *config,
                        const char_class_t nameclass, var_cb_t lookup,
                        void *lookup_context,
                        tokenbuf_t *result, int current_index,
                        int *rel_lookup_flag);

int substext_or_variable(const char *begin, const char *end,
                         const var_config_t *config,
                         const char_class_t nameclass, var_cb_t lookup,
                         void *lookup_context,
                         tokenbuf_t *result, int current_index,
                         int *rel_lookup_flag);

int transpose(tokenbuf_t *data, tokenbuf_t *search,
              tokenbuf_t *replace);

var_rc_t expand_character_class(const char *desc, char_class_t char_class);

int variable(const char *begin, const char *end,
             const var_config_t *config, const char_class_t nameclass,
             var_cb_t lookup, void *lookup_context,
             tokenbuf_t *result, int current_index,
             int *rel_lookup_flag);

int varname(const char *begin, const char *end,
            const char_class_t nameclass);

struct wrapper_context
    {
    var_cb_t lookup;
    void    *context;
    int     *rel_lookup_flag;
    };

var_rc_t lookup_wrapper(void* context,
                        const char* name, size_t name_len, int idx,
                        const char** data, size_t* data_len,
                        size_t* buffer_size);

int text(const char *begin, const char *end, char varinit,
         char startindex, char endindex, char escape);
