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

int append_to_tokenbuf(tokenbuf* output, const char* begin, int rc);

var_rc_t input(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* output);

#endif /* !defined(INTERNAL_H) */
