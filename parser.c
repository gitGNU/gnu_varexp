#include <assert.h>
#include "internal.h"

/*
    input           : ( TEXT | variable )*

    variable        : '$' ( VARNAME | expression )

    expression      : START-DELIM ( VARNAME | variable )+ ( ':' command )* END-DELIM

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

    NUMBER          : '[0-9]+'

    SUBSTTEXT       : '[^$/]'

    EXPTEXT         : '[^$}:]+'

    TEXT            : '[^$]+'
*/

int text(const char* begin, const char* end, char varinit, char escape)
    {
    const char* p;

    for (p = begin; p != end && *p != varinit; ++p)
        {
        if (*p == escape)
            {
            if (p+1 == end)
                return VAR_INCOMPLETE_QUOTED_PAIR;
            else
                ++p;
            }
        }

    return p - begin;
    }

int varname(const char* begin, const char* end, const char nameclass[256])
    {
    const char* p;
    for (p = begin; p != end && nameclass[(int)*p]; ++p)
        ;
    return p - begin;
    }

int expression(const char* begin, const char* end, const var_config_t* config, const char nameclass[256])
    {
    return -1;
    }

int variable(const char* begin, const char* end, const var_config_t* config,
             const char nameclass[256], var_cb_t lookup, void* lookup_context,
             int force_expand, tokenbuf* result)
    {
    const char* p = begin;
    int rc;

    if (p == end || *p != config->varinit)
        return 0;

    if (++p == end)
        return VAR_INCOMPLETE_VARIABLE_SPEC;

    rc = varname(p, end, nameclass);
    if (rc < 0)
        return rc;
    else if (rc > 0)
        {
        const char* data;
        size_t len, buffer_size;
        int rc2;
        rc2 = (*lookup)(lookup_context, p, rc, &data, &len, &buffer_size);
        if (rc2 < 0)
            return rc2;
        else if (rc2 == 0)
            {
            if (force_expand)
                return VAR_UNDEFINED_VARIABLE;
            else
                {
                result->begin       = begin;
                result->end         = begin + 1 + rc;
                result->buffer_size = 0;
                return 1 + rc;
                }
            }
        else
            {
            result->begin       = data;
            result->end         = data + len;
            result->buffer_size = buffer_size;
            return 1 + rc;
            }
        }

    return expression(begin, end, config, nameclass);
    }

var_rc_t input(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* output)
    {
    int rc;
    tokenbuf result;

    while(begin != end)
        {
        rc = text(begin, end, config->varinit, config->escape);
        if (rc > 0)
            {
            if (!append_to_tokenbuf(output, begin, rc))
                return VAR_OUT_OF_MEMORY;
            begin += rc;
            }
        else if (rc < 0)
            return rc;
        else
            {
            rc = variable(begin, end, config, nameclass, lookup, lookup_context,
                          force_expand, &result);
            if (rc > 0)
                {
                int rc2 = append_to_tokenbuf(output, result.begin, result.end - result.begin);
                if (result.buffer_size > 0)
                    free((char*)result.begin);
                if (!rc2)
                    return VAR_OUT_OF_MEMORY;
                else
                    begin += rc;
                }
            else if (rc < 0)
                return rc;
            else
                return VAR_INPUT_ISNT_TEXT_NOR_VARIABLE;
            }
        }

    return VAR_OK;
    }

int name()
    {
    return -1;
    }

int command()
    {
    return -1;
    }

int start_delim()
    {
    return -1;
    }

int end_delim()
    {
    return -1;
    }

int number()
    {
    return -1;
    }

int substext()
    {
    return -1;
    }

int exptext()
    {
    return -1;
    }
