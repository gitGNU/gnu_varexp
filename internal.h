#ifndef INTERNAL_H
#define INTERNAL_H

#include "varexp.h"

typedef enum
    {
    VAR_INCOMPLETE_GROUPED_HEX      = -7,
    VAR_INCOMPLETE_OCTAL            = -6,
    VAR_INVALID_OCTAL               = -5,
    VAR_OCTAL_TOO_LARGE             = -4,
    VAR_INVALID_HEX                 = -3,
    VAR_INCOMPLETE_HEX              = -2,
    VAR_INCOMPLETE_NAMED_CHARACTER  = -1,
    VAR_OK                          = 0
    }
var_rc_t;

var_rc_t expand_named_characters(char* buffer, size_t len);

#endif /* !defined(INTERNAL_H) */