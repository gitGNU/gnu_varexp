#include <sys/types.h>
#include <regex.h>
#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
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
                        dst->append((char*)&c, 1);
                        }
                    p += 3;
                    }
                else
                    {
                    dst->append(p, 1);
                    ++p;
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

            data->force_copy();

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

            return VAR_OK;

          error_return:
            search->clear();
            replace->clear();
            return rc;
            }
        }
    }
