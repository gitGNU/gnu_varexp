#include <sys/types.h>
#include <regex.h>
#include "internal.h"

static int expand_class_description(tokenbuf* src, tokenbuf* dst)
    {
    unsigned char c, d;
    const char* p = src->begin;
    while(p != src->end)
        {
        if ((src->end - p) >= 3 && p[1] == '-')
            {
            printf("Expand class.\n");
            if (*p > p[2])
                return VAR_INCORRECT_TRANSPOSE_CLASS_SPEC;
            for (c = *p, d = p[2]; c <= d; ++c)
                {
                if (!append_to_tokenbuf(dst, (char*)&c, 1))
                    return VAR_OUT_OF_MEMORY;
                }
            p += 3;
            }
        else
            {
            printf("Copy verbatim.\n");
            if (!append_to_tokenbuf(dst, p, 1))
                return VAR_OUT_OF_MEMORY;
            else
                ++p;
            }
        }
    return VAR_OK;
    }

int transpose(tokenbuf* data, tokenbuf* search, tokenbuf* replace)
    {
    tokenbuf srcclass, dstclass;
    const char* p;
    int rc;
    size_t i;

    init_tokenbuf(&srcclass);
    init_tokenbuf(&dstclass);

    if ((rc = expand_class_description(search, &srcclass)) != VAR_OK)
        goto error_return;
    if ((rc = expand_class_description(replace, &dstclass)) != VAR_OK)
        goto error_return;

    printf("Transpose from '%s' to '%s'.\n",
           srcclass.begin, dstclass.begin);

    if (srcclass.begin == srcclass.end)
        {
        rc = VAR_EMPTY_TRANSPOSE_CLASS;
        goto error_return;
        }
    if ((srcclass.end - srcclass.begin) != (dstclass.end - dstclass.begin))
        {
        rc = VAR_TRANSPOSE_CLASSES_MISMATCH;
        goto error_return;
        }

    if (data->buffer_size == 0)
        {
        tokenbuf tmp;
        if (!assign_to_tokenbuf(&tmp, data->begin, data->end - data->begin))
            {
            rc = VAR_OUT_OF_MEMORY;
            goto error_return;
            }
        move_tokenbuf(&tmp, data);
        }

    for (p = data->begin; p != data->end; ++p)
        {
        for (i = 0; i <= (srcclass.end - srcclass.begin); ++i)
            {
            if (*p == srcclass.begin[i])
                {
                *((char*)p) = dstclass.begin[i];
                break;
                }
            }
        }

    free_tokenbuf(&srcclass);
    free_tokenbuf(&dstclass);
    return VAR_OK;

  error_return:
    free_tokenbuf(search);
    free_tokenbuf(replace);
    free_tokenbuf(&srcclass);
    free_tokenbuf(&dstclass);
    return rc;
    }
