#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "internal.h"

#include <stdio.h>

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

int expression(const char* begin, const char* end, const var_config_t* config,
               const char nameclass[256], var_cb_t lookup, void* lookup_context,
               int force_expand, tokenbuf* result)
    {
    /*     expression      : START-DELIM ( VARNAME | variable )+ ( ':' command )* END-DELIM */

    const char* p = begin;
    const char* data;
    size_t len, buffer_size;
    int failed = 0;
    int rc;
    tokenbuf name;

    printf("Finding expression() in '%s'.\n", begin);

    if (p == end || *p != config->startdelim)
        return 0;

    if (++p == end)
        return VAR_INCOMPLETE_VARIABLE_SPEC;

    name.begin = name.end = NULL;
    name.buffer_size = 0;
    do
        {
        printf("name of the variable is '%s'\n", name.begin);

        rc = varname(p, end, nameclass);
        if (rc < 0)
            {
            free_tokenbuf(&name);
            return rc;
            }
        else if (rc > 0)
            {
            if (!append_to_tokenbuf(&name, p, rc))
                {
                free_tokenbuf(&name);
                return VAR_OUT_OF_MEMORY;
                }
            else
                p += rc;
            }
        else
            {
            tokenbuf tmp;

            rc = variable(p, end, config, nameclass, lookup, lookup_context,
                          force_expand, &tmp);
            if (rc < 0)
                {
                free_tokenbuf(&name);
                return rc;
                }
            else if (rc > 0)
                {
                if (!append_to_tokenbuf(&name, tmp.begin, tmp.end - tmp.begin))
                    {
                    free_tokenbuf(&name);
                    free_tokenbuf(&tmp);
                    return VAR_OUT_OF_MEMORY;
                    }
                else
                    p += rc;
                }
            }
        }
    while (rc > 0);

    printf("Scanning for expression() is over at '%s'.\n", p);

    if (p == end || (*p != config->enddelim && *p != ':'))
        {
        free_tokenbuf(&name);
        return VAR_INCOMPLETE_VARIABLE_SPEC;
        }
    else
        ++p;

    if (name.begin == name.end)
        {
        free_tokenbuf(&name);
        return VAR_INCOMPLETE_VARIABLE_SPEC;
        }

    rc = (*lookup)(lookup_context, name.begin, name.end - name.begin, &data, &len, &buffer_size);
    free_tokenbuf(&name);
    if (rc < 0)
        return rc;
    else if (rc == 0)
        {
        if (force_expand)
            return VAR_UNDEFINED_VARIABLE;
        else
            {
            result->begin       = begin-1;
            result->end         = p;
            result->buffer_size = 0;
            failed = 1;
            }
        }
    else
        {
        result->begin       = data;
        result->end         = data + len;
        result->buffer_size = buffer_size;
        }

    if (p[-1] == ':')
        {
        /* Still commands to parse and execute. */

        tokenbuf* tmp;

        if (failed)
            {
            name.begin = name.end = NULL;
            name.buffer_size = 0;
            tmp = &name;
            }
        else
            tmp = result;

        --p;
        while (p != end && *p == ':')
            {
            ++p;
            rc = command(p, end, config, nameclass, lookup, lookup_context,
                         force_expand, tmp);
            if (rc < 0)
                {
                free_tokenbuf(result);
                return rc;
                }
            else
                p += rc;
            }

        if (p == end || *p != config->enddelim)
            {
            free_tokenbuf(result);
            return VAR_INCOMPLETE_VARIABLE_SPEC;
            }
        else
            ++p;
        }

    return p - begin;
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
    else
        {
        rc = expression(p, end, config, nameclass, lookup, lookup_context,
                        force_expand, result);
        if (rc > 0)
            ++rc;
        return rc;
        }
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
                free_tokenbuf(&result);
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

int command(const char* begin, const char* end, const var_config_t* config,
            const char nameclass[256], var_cb_t lookup, void* lookup_context,
            int force_expand, tokenbuf* data)
    {
    /*
      command         : '-' ( EXPTEXT | variable )*
      | '?' ( EXPTEXT | variable )*
      | 'o' ( NUMBER '-' ( NUMBER )? | NUMBER ',' ( NUMBER )? )
      | '*' ( EXPTEXT | variable )*
      | 's' '/' ( variable | SUBSTTEXT )+ '/' ( variable | SUBSTTEXT )+ '/' ( 'g' | 'i' | 't' )*
      | 'y' '/' ( variable | SUBSTTEXT )+ '/' ( variable | SUBSTTEXT )+ '/'
    */

    const char* p = begin;
    char* tmpptr;
    char  tmpbuf[1024];
    tokenbuf tmptokbuf;
    int rc;

    printf("Entering command(); data is '");
    if (data->begin)
        fwrite(data->begin, data->end - data->begin, 1, stdout);
    printf("'; input is '%s'.\n", begin);

    if (begin == end)
        return 0;

    switch (tolower(*p))
        {
        case 'l':               /* Turn data to lowercase. */
            if (data->begin)
                {
                if (data->buffer_size == 0)
                    {
                    tokenbuf tmp;
                    tmp.begin = tmp.end = NULL;
                    tmp.buffer_size = 0;
                    if (!append_to_tokenbuf(&tmp, data->begin, data->end - data->begin))
                        return VAR_OUT_OF_MEMORY;
                    data->begin = tmp.begin;
                    data->end = tmp.end;
                    data->buffer_size = tmp.buffer_size;
                    }
                for (tmpptr = (char*)data->begin; tmpptr != data->end; ++tmpptr)
                    *tmpptr = tolower(*tmpptr);
                }
            ++p;
            break;

        case 'u':               /* Turn data to uppercase. */
            if (data->begin)
                {
                if (data->buffer_size == 0)
                    {
                    printf("*** We need to copy the buffer before calling toupper().\n");
                    tokenbuf tmp;
                    tmp.begin = tmp.end = NULL;
                    tmp.buffer_size = 0;
                    if (!append_to_tokenbuf(&tmp, data->begin, data->end - data->begin))
                        return VAR_OUT_OF_MEMORY;
                    data->begin = tmp.begin;
                    data->end = tmp.end;
                    data->buffer_size = tmp.buffer_size;
                    }
                for (tmpptr = (char*)data->begin; tmpptr != data->end; ++tmpptr)
                    *tmpptr = toupper(*tmpptr);
                }
            ++p;
            break;

        case '#':               /* Substitute length of the string. */
            if (data->begin)
                {
                sprintf(tmpbuf, "%d", data->end - data->begin);
                free_tokenbuf(data);
                append_to_tokenbuf(data, tmpbuf, strlen(tmpbuf));
                }
            ++p;
            break;

        case '-':               /* Substitute parameter if data is empty. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                return rc;
            else
                p += rc;
            if (data->begin != NULL && data->begin == data->end)
                {
                free_tokenbuf(data);
                data->begin = tmptokbuf.begin;
                data->end = tmptokbuf.end;
                data->buffer_size = tmptokbuf.buffer_size;
                }
            break;

        case '?':               /* Return parameter as error string if data is empty. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                return rc;
            else
                p += rc;
            if (data->begin != NULL)
                {
                if (data->begin == data->end)
                    {
                    printf("We are returning error string '%s'.\n", tmptokbuf.begin);
                    free_tokenbuf(&tmptokbuf);
                    free_tokenbuf(data);
                    return VAR_USER_ABORT;
                    }
                }
            free_tokenbuf(&tmptokbuf);
            break;

        case '*':               /* Return "" if data is not empty, parameter otherwise. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                return rc;
            else
                p += rc;
            if (data->begin != NULL)
                {
                if (data->begin == data->end)
                    {
                    free_tokenbuf(data);
                    data->begin = tmptokbuf.begin;
                    data->end = tmptokbuf.end;
                    data->buffer_size = tmptokbuf.buffer_size;
                    }
                else
                    {
                    free_tokenbuf(data);
                    data->begin = tmptokbuf.begin;
                    data->end = tmptokbuf.begin;
                    data->buffer_size = tmptokbuf.buffer_size;
                    }
                }
            else
                free_tokenbuf(&tmptokbuf);
            break;

        case '+':               /* Substitute ${parameter} if data is not empty. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                return rc;
            else
                p += rc;
            if (data->begin != NULL)
                {
                if (data->begin != data->end)
                    {
                    const char* vardata;
                    size_t len, buffer_size;
                    free_tokenbuf(data);
                    printf("*** looking up contents of '%s'\n", tmptokbuf.begin);
                    rc = (*lookup)(lookup_context, tmptokbuf.begin, tmptokbuf.end - tmptokbuf.begin,
                                   &vardata, &len, &buffer_size);
                    if (rc < 0)
                        {
                        free_tokenbuf(&tmptokbuf);
                        return rc;
                        }
                    else if (rc == 0)
                        {
                        if (force_expand)
                            {
                            free_tokenbuf(&tmptokbuf);
                            return VAR_UNDEFINED_VARIABLE;
                            }
                        else
                            {
                            if (!append_to_tokenbuf(data, &(config->varinit), 1) ||
                                !append_to_tokenbuf(data, &(config->startdelim), 1) ||
                                !append_to_tokenbuf(data, tmptokbuf.begin, tmptokbuf.end - tmptokbuf.begin) ||
                                !append_to_tokenbuf(data, &(config->enddelim), 1))
                                {
                                free_tokenbuf(data);
                                free_tokenbuf(&tmptokbuf);
                                return VAR_OUT_OF_MEMORY;
                                }
                            }
                        }
                    else
                        {
                        printf("***  '%s' --> '%s'\n", tmptokbuf.begin, vardata);
                        data->begin = vardata;
                        data->end = vardata + len;
                        data->buffer_size = buffer_size;
                        }
                    }
                }
            free_tokenbuf(&tmptokbuf);
            break;

        default:
            return VAR_UNKNOWN_COMMAND_CHAR;
        }

    printf("Leaving command(); data is '");
    if (data->begin)
        fwrite(data->begin, data->end - data->begin, 1, stdout);
    printf("'.\n");

    return p - begin;
    }

int number(const char* begin, const char* end)
    {
    const char* p;
    for (p = begin; p != end && isdigit(*p); ++p)
        ;
    return p - begin;
    }

int substext(const char* begin, const char* end, const var_config_t* config)
    {
    const char* p;
    for (p = begin; p != end && *p != config->varinit && *p != ':'; ++p)
        {
        if (*p == config->escape)
            {
            if (p+1 == end)
                return VAR_INCOMPLETE_QUOTED_PAIR;
            else
                ++p;
            }
        }
    return p - begin;
    }

int exptext(const char* begin, const char* end, const var_config_t* config)
    {
    const char* p;
    for (p = begin;
         p != end
             && *p != config->varinit
             && *p != config->enddelim
             && *p != ':';
         ++p)
        {
        if (*p == config->escape)
            {
            if (p+1 == end)
                return VAR_INCOMPLETE_QUOTED_PAIR;
            else
                ++p;
            }
        }
    return p - begin;
    }

int exptext_or_variable(const char* begin, const char* end, const var_config_t* config,
                        const char nameclass[256], var_cb_t lookup, void* lookup_context,
                        int force_expand, tokenbuf* result)
    {
    const char* p = begin;
    tokenbuf tmp;
    int rc;

    if (begin == end)
        return 0;

    result->begin = result->end = NULL;
    result->buffer_size = 0;

    do
        {
        rc = exptext(p, end, config);
        if (rc < 0)
            {
            free_tokenbuf(result);
            return rc;
            }
        else if (rc > 0)
            {
            printf("*** exptext found %d characters'.\n", rc);
            if (!append_to_tokenbuf(result, p, rc))
                {
                free_tokenbuf(result);
                return VAR_OUT_OF_MEMORY;
                }
            else
                p += rc;
            }

        rc = variable(p, end, config, nameclass, lookup, lookup_context, force_expand, &tmp);
        if (rc < 0)
            {
            free_tokenbuf(result);
            return rc;
            }
        else if (rc > 0)
            {
            printf("*** variable expanded to '%s'.\n", tmp.begin);
            p += rc;
            rc = append_to_tokenbuf(result, tmp.begin, tmp.end - tmp.begin);
            free_tokenbuf(&tmp);
            if (!rc)
                {
                free_tokenbuf(result);
                return VAR_OUT_OF_MEMORY;
                }
            }
        }
    while (rc > 0);


    return p - begin;
    }

int substext_or_variable(const char* begin, const char* end, const var_config_t* config,
                         const char nameclass[256], var_cb_t lookup, void* lookup_context,
                         int force_expand, tokenbuf* result)
    {
    const char* p = begin;
    tokenbuf tmp;
    int rc;

    if (begin == end)
        return 0;

    result->begin = result->end = NULL;
    result->buffer_size = 0;

    do
        {
        rc = substext(p, end, config);
        if (rc < 0)
            {
            free_tokenbuf(result);
            return rc;
            }
        else if (rc > 0)
            {
            printf("*** substext found %d characters'.\n", rc);
            if (!append_to_tokenbuf(result, p, rc))
                {
                free_tokenbuf(result);
                return VAR_OUT_OF_MEMORY;
                }
            else
                p += rc;
            }

        rc = variable(p, end, config, nameclass, lookup, lookup_context, force_expand, &tmp);
        if (rc < 0)
            {
            free_tokenbuf(result);
            return rc;
            }
        else if (rc > 0)
            {
            printf("*** variable expanded to '%s'.\n", tmp.begin);
            p += rc;
            rc = append_to_tokenbuf(result, tmp.begin, tmp.end - tmp.begin);
            free_tokenbuf(&tmp);
            if (!rc)
                {
                free_tokenbuf(result);
                return VAR_OUT_OF_MEMORY;
                }
            }
        }
    while (rc > 0);


    return p - begin;
    }
