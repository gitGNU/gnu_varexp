#include <assert.h>
#include "internal.h"

/*
    input           : ( TEXT | variable )*

    variable        : '$' ( name | expression )

    expression      : START-DELIM ( name | variable )+ ( ':' command )* END-DELIM

    name            : ( VARNAME | SPECIAL1 | SPECIAL2 )+

    command         : '-' ( EXPTEXT | variable )*
                    | '?' ( EXPTEXT | variable )*
                    | '+' ( EXPTEXT | variable )*
                    | 'o' ( NUMBER '-' ( NUMBER )? | NUMBER ',' ( NUMBER )? )
                    | '#'
                    | '*' ( EXPTEXT | variable )*
                    | 's' '/' ( variable | SUBSTTEXT )+ '/' ( variable | SUBSTTEXT )+ '/' ( 'g' | 'i' | 't' )*
                    | 'y' '/' ( variable | SUBSTTEXT )+ '/' ( variable | SUBSTTEXT )+ '/'
                    | 'l'
                    | 'u'

    START-DELIM     : '{'

    END-DELIM       : '}'

    VARNAME         : '[a-zA-Z0-9_]+'

    SPECIAL1        : '['

    SPECIAL2        : ']'

    NUMBER          : '[0-9]+'

    SUBSTTEXT       : '[^$/]'

    EXPTEXT         : '[^$}:]+'

    TEXT            : '[^$]+'
*/

int TEXT(const char* begin, const char* end, token* t)
    {
    const char* p;

    assert(begin != NULL);
    assert(end != NULL);
    assert(begin <= end);
    assert(t != NULL);

    if (begin == end || *begin == '$')
        return 0;

    for (p = begin; p < end && *p != '$'; ++p)
        ;

    t->begin       = begin;
    t->end         = p;
    t->buffer_size = 0;
    return 1;
    }
