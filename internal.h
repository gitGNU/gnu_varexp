#ifndef INTERNAL_H
#define INTERNAL_H

#include "varexp.h"

/* Turn character class descriptions into a lookup-array. */

var_rc_t expand_character_class(const char* desc, char class[256]);

/*
   The tokenbuf structure is used by the parser routines. If
   buffer_size is >0, it means that the buffer has been allocated by
   malloc(3) and must be free(3)ed when not used anymore.
*/

typedef struct
    {
    const char*  begin;
    const char*  end;
    size_t buffer_size;
    }
tokenbuf;

int append_to_tokenbuf(tokenbuf* output, const char* begin, size_t rc);
void free_tokenbuf(tokenbuf* buf);
void init_tokenbuf(tokenbuf* buf);
int assign_to_tokenbuf(tokenbuf* buf, const char* data, size_t len);
void move_tokenbuf(tokenbuf* src, tokenbuf* dst);

var_rc_t input(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* output);

int variable(const char* begin, const char* end, const var_config_t* config,
             const char nameclass[256], var_cb_t lookup, void* lookup_context,
             int force_expand, tokenbuf* result);


int command(const char* begin, const char* end, const var_config_t* config,
            const char nameclass[256], var_cb_t lookup, void* lookup_context,
            int force_expand, tokenbuf* result);

int exptext(const char* begin, const char* end, const var_config_t* config);

int exptext_or_variable(const char* begin, const char* end, const var_config_t* config,
                        const char nameclass[256], var_cb_t lookup, void* lookup_context,
                        int force_expand, tokenbuf* result);


int substext_or_variable(const char* begin, const char* end, const var_config_t* config,
                         const char nameclass[256], var_cb_t lookup, void* lookup_context,
                         int force_expand, tokenbuf* result);

int substext(const char* begin, const char* end, const var_config_t* config);

int search_and_replace(tokenbuf* data, tokenbuf* search, tokenbuf* replace, tokenbuf* flags);

int varname(const char* begin, const char* end, const char nameclass[256]);

int expression(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* result);

int text(const char* begin, const char* end, char varinit, char escape);

int number(const char* begin, const char* end);

int cut_out_offset(tokenbuf* data, tokenbuf* number1, tokenbuf* number2, int isrange);

#endif /* !defined(INTERNAL_H) */
