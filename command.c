#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "internal.h"

#include <stdio.h>

int command(const char* begin, const char* end, const var_config_t* config,
            const char nameclass[256], var_cb_t lookup, void* lookup_context,
            int force_expand, tokenbuf* data)
    {
    const char* p = begin;
    tokenbuf tmptokbuf;
    tokenbuf search, replace, flags;
    int rc;

    init_tokenbuf(&tmptokbuf);
    init_tokenbuf(&search);
    init_tokenbuf(&replace);
    init_tokenbuf(&flags);

    if (begin == end)
        return 0;

    switch (tolower(*p))
        {
        case 'l':               /* Turn data to lowercase. */
            if (data->begin)
                {
                char* ptr;
                /* If the buffer does not life in an allocated buffer,
                   we have to copy it before modifying the contents. */

                if (data->buffer_size == 0)
                    {
                    if (!assign_to_tokenbuf(data, data->begin, data->end - data->begin))
                        {
                        rc = VAR_OUT_OF_MEMORY;
                        goto error_return;
                        }
                    }
                for (ptr = (char*)data->begin; ptr != data->end; ++ptr)
                    *ptr = tolower(*ptr);
                }
            ++p;
            break;

        case 'u':               /* Turn data to uppercase. */
            if (data->begin)
                {
                char* ptr;
                if (data->buffer_size == 0)
                    {
                    if (!assign_to_tokenbuf(data, data->begin, data->end - data->begin))
                        {
                        rc = VAR_OUT_OF_MEMORY;
                        goto error_return;
                        }
                    }
                for (ptr = (char*)data->begin; ptr != data->end; ++ptr)
                    *ptr = toupper(*ptr);
                }
            ++p;
            break;

        case '#':               /* Substitute length of the string. */
            if (data->begin)
                {
                char buf[1024];
                sprintf(buf, "%d", data->end - data->begin);
                free_tokenbuf(data);
                if (!assign_to_tokenbuf(data, buf, strlen(buf)))
                    {
                    rc = VAR_OUT_OF_MEMORY;
                    goto error_return;
                    }
                }
            ++p;
            break;

        case '-':               /* Substitute parameter if data is empty. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                goto error_return;
            else if (rc == 0)
                {
                rc = VAR_MISSING_PARAMETER_IN_COMMAND;
                goto error_return;
                }
            else
                p += rc;
            if (data->begin != NULL && data->begin == data->end)
                {
                free_tokenbuf(data);
                move_tokenbuf(&tmptokbuf, data);
                }
            break;

        case '?':               /* Return parameter as error string if data is empty. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                goto error_return;
            else if (rc == 0)
                {
                rc = VAR_MISSING_PARAMETER_IN_COMMAND;
                goto error_return;
                }
            else
                p += rc;
            if (data->begin != NULL)
                {
                if (data->begin == data->end)
                    {
                    printf("We are returning error string '%s'.\n", tmptokbuf.begin);
                    rc = VAR_USER_ABORT;
                    goto error_return;
                    }
                }
            break;

        case '*':               /* Return "" if data is not empty, parameter otherwise. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                goto error_return;
            else if (rc == 0)
                {
                rc = VAR_MISSING_PARAMETER_IN_COMMAND;
                goto error_return;
                }
            else
                p += rc;
            if (data->begin != NULL)
                {
                if (data->begin == data->end)
                    {
                    free_tokenbuf(data);
                    move_tokenbuf(&tmptokbuf, data);
                    }
                else
                    {
                    free_tokenbuf(data);
                    data->begin = data->end = "";
                    data->buffer_size = 0;
                    }
                }
            break;

        case '+':               /* Substitute ${parameter} if data is not empty. */
            ++p;
            rc = exptext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                     force_expand, &tmptokbuf);
            if (rc < 0)
                goto error_return;
            else if (rc == 0)
                {
                rc = VAR_MISSING_PARAMETER_IN_COMMAND;
                goto error_return;
                }
            else
                p += rc;
            if (data->begin != NULL)
                {
                if (data->begin != data->end)
                    {
                    const char* vardata;
                    size_t len, buffer_size;
                    free_tokenbuf(data);
                    rc = (*lookup)(lookup_context, tmptokbuf.begin, tmptokbuf.end - tmptokbuf.begin,
                                   &vardata, &len, &buffer_size);
                    if (rc < 0)
                        goto error_return;
                    else if (rc == 0)
                        {
                        if (force_expand)
                            {
                            rc = VAR_UNDEFINED_VARIABLE;
                            goto error_return;
                            }
                        else
                            {
                            if (!assign_to_tokenbuf(data, &(config->varinit), 1) ||
                                !append_to_tokenbuf(data, &(config->startdelim), 1) ||
                                !append_to_tokenbuf(data, tmptokbuf.begin, tmptokbuf.end - tmptokbuf.begin) ||
                                !append_to_tokenbuf(data, &(config->enddelim), 1))
                                {
                                rc = VAR_OUT_OF_MEMORY;
                                goto error_return;
                                }
                            }
                        }
                    else
                        {
                        data->begin = vardata;
                        data->end = vardata + len;
                        data->buffer_size = buffer_size;
                        }
                    }
                }
            break;

        case 's':               /* Search and replace. */
            ++p;

            if (*p != '/')
                return VAR_MALFORMATTED_REPLACE;
            else
                ++p;

            rc = substext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                      force_expand, &search);
            if (rc < 0)
                goto error_return;
            else
                p += rc;

            if (*p != '/')
                {
                rc = VAR_MALFORMATTED_REPLACE;
                goto error_return;
                }
            else
                ++p;

            rc = substext_or_variable(p, end, config, nameclass, lookup, lookup_context,
                                      force_expand, &replace);
            if (rc < 0)
                goto error_return;
            else
                p += rc;

            if (*p != '/')
                {
                rc = VAR_MALFORMATTED_REPLACE;
                goto error_return;
                }
            else
                ++p;

            rc = exptext(p, end, config);
            if (rc < 0)
                goto error_return;
            else
                {
                flags.begin = p;
                flags.end = p + rc;
                flags.buffer_size = 0;
                p += rc;
                }

            rc = search_and_replace(data, &search, &replace, &flags);
            if (rc < 0)
                goto error_return;
            break;

        default:
            return VAR_UNKNOWN_COMMAND_CHAR;
        }

    /* Exit gracefully. */

    free_tokenbuf(&tmptokbuf);
    free_tokenbuf(&search);
    free_tokenbuf(&replace);
    free_tokenbuf(&flags);
    return p - begin;

  error_return:
    free_tokenbuf(data);
    free_tokenbuf(&tmptokbuf);
    free_tokenbuf(&search);
    free_tokenbuf(&replace);
    free_tokenbuf(&flags);
    return rc;
    }