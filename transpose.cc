#include <sys/types.h>
#include <regex.h>
#include "internal.h"

static int expand_class_description(tokenbuf_t* src, tokenbuf_t* dst)
    {
    unsigned char c, d;
    const char* p = src->begin;

    while (p != src->end)
        {
        if ((src->end - p) >= 3 && p[1] == '-')
            {
            if (*p > p[2])
                return VAR_ERR_INCORRECT_TRANSPOSE_CLASS_SPEC;
            for (c = *p, d = p[2]; c <= d; ++c)
                {
                if (!tokenbuf_append(dst, (char*)&c, 1))
                    return VAR_ERR_OUT_OF_MEMORY;
                }
            p += 3;
            }
        else
            {
            if (!tokenbuf_append(dst, p, 1))
                return VAR_ERR_OUT_OF_MEMORY;
            p++;
            }
        }
    return VAR_OK;
    }

int transpose(tokenbuf_t* data, tokenbuf_t* search,
              tokenbuf_t* replace)
    {
    tokenbuf_t srcclass, dstclass;
    const char* p;
    int rc;
    size_t i;

    tokenbuf_init(&srcclass);
    tokenbuf_init(&dstclass);

    if ((rc = expand_class_description(search, &srcclass)) != VAR_OK)
        goto error_return;
    if ((rc = expand_class_description(replace, &dstclass)) != VAR_OK)
        goto error_return;

    if (srcclass.begin == srcclass.end)
        {
        rc = VAR_ERR_EMPTY_TRANSPOSE_CLASS;
        goto error_return;
        }
    if ((srcclass.end - srcclass.begin) != (dstclass.end - dstclass.begin))
        {
        rc = VAR_ERR_TRANSPOSE_CLASSES_MISMATCH;
        goto error_return;
        }

    if (data->buffer_size == 0)
        {
        tokenbuf_t tmp;
        if (!tokenbuf_assign(&tmp, data->begin, data->end - data->begin))
            {
            rc = VAR_ERR_OUT_OF_MEMORY;
            goto error_return;
            }
        tokenbuf_move(&tmp, data);
        }

    for (p = data->begin; p != data->end; ++p)
        {
        for (i = 0; i <= (size_t)(srcclass.end - srcclass.begin); ++i)
            {
            if (*p == srcclass.begin[i])
                {
                *((char*) p) = dstclass.begin[i];
                break;
                }
            }
        }

    tokenbuf_free(&srcclass);
    tokenbuf_free(&dstclass);
    return VAR_OK;

  error_return:
    tokenbuf_free(search);
    tokenbuf_free(replace);
    tokenbuf_free(&srcclass);
    tokenbuf_free(&dstclass);
    return rc;
    }
