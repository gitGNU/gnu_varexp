#include <sys/types.h>
#include <regex.h>
#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        static void expand_class_description(tokenbuf_t* src, tokenbuf_t* dst)
            {
            unsigned char c, d;
            const char* p = src->begin;

            while (p != src->end)
                {
                if ((src->end - p) >= 3 && p[1] == '-')
                    {
                    if (*p > p[2])
                        throw incorrect_transpose_class_spec();
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
            }

        void transpose(tokenbuf_t* data, tokenbuf_t* search,
                      tokenbuf_t* replace)
            {
            tokenbuf_t srcclass, dstclass;
            const char* p;
            size_t i;

            expand_class_description(search, &srcclass);
            expand_class_description(replace, &dstclass);

            if (srcclass.begin == srcclass.end)
                {
                throw empty_transpose_class();
                }
            if ((srcclass.end - srcclass.begin) != (dstclass.end - dstclass.begin))
                {
                throw transpose_classes_mismatch();
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
            }
        }
    }
