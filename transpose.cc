#include <sys/types.h>
#include <regex.h>
#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        inline void expand_class_description(const string& src, string& dst)
            {
            for (string::const_iterator p = src.begin(); p != src.end(); )
                {
                if (src.end() - p >= 3 && p[1] == '-')
                    {
                    if (*p > p[2])
                        throw incorrect_transpose_class_spec();
                    unsigned c, d;
                    for (c = *p, d = p[2]; c <= d; ++c)
                        {
                        dst.append(1, static_cast<string::value_type>(c));
                        }
                    p += 3;
                    }
                else
                    {
                    dst.append(1, *p);
                    ++p;
                    }
                }
            }

        void transpose(std::string& data, const std::string& srcdesc, const std::string& dstdesc)
            {
            string srcclass, dstclass;

            expand_class_description(srcdesc, srcclass);
            expand_class_description(dstdesc, dstclass);

            if (srcclass.empty())
                throw empty_transpose_class();
            if (srcclass.size() != dstclass.size())
                throw transpose_classes_mismatch();

            for (string::iterator p = data.begin(); p != data.end(); ++p)
                {
                for (size_t i = 0; i <= srcclass.size(); ++i)
                    {
                    if (*p == srcclass[i])
                        {
                        *p = dstclass[i];
                        break;
                        }
                    }
                }
            }
        }
    }
