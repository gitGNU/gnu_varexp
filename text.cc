#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t parser::text(const char *begin, const char *end)
            {
            const char* p;

            for (p = begin; p != end; ++p)
                {
                if (*p == config.escape)
                    {
                    if (++p == end)
                        throw incomplete_quoted_pair();
                    }
                else if (*p == config.varinit)
                    break;
                else if (config.startindex && (*p == config.startindex || *p == config.endindex))
                    break;
                }
            return p - begin;
            }

        size_t parser::varname(const char* begin, const char* end)
            {
            const char* p;

            for (p = begin; p != end && nameclass[(int) *p]; p++)
                ;
            return p - begin;
            }

        size_t parser::number(const char *begin, const char *end, unsigned int& num)
            {
            const char* p = begin;
            num = 0;

            while (isdigit(*p) && p != end)
                {
                num *= 10;
                num += *p - '0';
                ++p;
                }
            return p - begin;
            }

        size_t parser::substext(const char* begin, const char* end)
            {
            const char* p;

            for (p = begin; p != end && *p != config.varinit && *p != '/'; p++)
                {
                if (*p == config.escape)
                    {
                    if (p + 1 == end)
                        throw incomplete_quoted_pair();
                    p++;
                    }
                }
            return p - begin;
            }

        size_t parser::exptext(const char *begin, const char *end)
            {
            const char* p;

            for (p = begin;
                 p != end && *p != config.varinit && *p != config.enddelim && *p != ':';
                 ++p)
                {
                if (*p == config.escape)
                    {
                    if (p + 1 == end)
                        throw incomplete_quoted_pair();
                    p++;
                    }
                }
            return p - begin;
            }

        size_t parser::exptext_or_variable(const char *begin, const char *end, string& result)
            {
            const char* p = begin;
            size_t rc;

            if (begin == end)
                return 0;

            do
                {
                rc = exptext(p, end);
                if (rc > 0)
                    {
                    result.append(p, rc);
                    p += rc;
                    }

                string tmp;
                rc = variable(p, end, tmp);
                if (rc > 0)
                    {
                    result.append(tmp);
                    p += rc;
                    }
                }
            while (rc > 0);

            return p - begin;
            }

        size_t parser::substext_or_variable(const char *begin, const char *end, string& result)
            {
            const char* p = begin;
            size_t rc;

            if (begin == end)
                return 0;

            do
                {
                rc = substext(p, end);
                if (rc > 0)
                    {
                    result.append(p, rc);
                    p += rc;
                    }

                string tmp;
                rc = variable(p, end, tmp);
                if (rc > 0)
                    {
                    result.append(tmp);
                    p += rc;
                    }
                }
            while (rc > 0);

            return p - begin;
            }
        }
    }
