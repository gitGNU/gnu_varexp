#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "internal.h"

#include <stdio.h>
#include <dmalloc.h>

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
    for (p = begin; p != end && *p != config->varinit && *p != '/'; ++p)
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
    for (p = begin; p != end && *p != config->varinit && *p != config->enddelim && *p != ':'; ++p)
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

    init_tokenbuf(result);
    init_tokenbuf(&tmp);

    if (begin == end)
        return 0;

    do
        {
        rc = exptext(p, end, config);
        if (rc < 0)
            goto error_return;
        else if (rc > 0)
            {
            if (!append_to_tokenbuf(result, p, rc))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            else
                p += rc;
            }

        rc = variable(p, end, config, nameclass, lookup, lookup_context, force_expand, &tmp);
        if (rc < 0)
            goto error_return;
        else if (rc > 0)
            {
            p += rc;
            if (!append_to_tokenbuf(result, tmp.begin, tmp.end - tmp.begin))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            }
        }
    while (rc > 0);

    free_tokenbuf(&tmp);
    return p - begin;

  error_return:
    free_tokenbuf(&tmp);
    free_tokenbuf(result);
    return rc;
    }

int substext_or_variable(const char* begin, const char* end, const var_config_t* config,
                         const char nameclass[256], var_cb_t lookup, void* lookup_context,
                         int force_expand, tokenbuf* result)
    {
    const char* p = begin;
    tokenbuf tmp;
    int rc;

    init_tokenbuf(result);
    init_tokenbuf(&tmp);

    if (begin == end)
        return 0;

    do
        {
        rc = substext(p, end, config);
        if (rc < 0)
            goto error_return;
        else if (rc > 0)
            {
            if (!append_to_tokenbuf(result, p, rc))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            else
                p += rc;
            }

        rc = variable(p, end, config, nameclass, lookup, lookup_context, force_expand, &tmp);
        if (rc < 0)
            goto error_return;
        else if (rc > 0)
            {
            p += rc;
            if (!append_to_tokenbuf(result, tmp.begin, tmp.end - tmp.begin))
                {
                rc = VAR_OUT_OF_MEMORY;
                goto error_return;
                }
            }
        }
    while (rc > 0);

    free_tokenbuf(&tmp);
    return p - begin;

  error_return:
    free_tokenbuf(&tmp);
    free_tokenbuf(result);
    return rc;
    }
