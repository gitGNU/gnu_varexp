#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t text(const char* begin, const char* end, char varinit,
                    char startindex, char endindex, char escape)
            {
            const char* p;

            for (p = begin; p != end; ++p)
                {
                if (*p == escape)
                    {
                    if (++p == end)
                        throw incomplete_quoted_pair();
                    }
                else if (*p == varinit)
                    break;
                else if (startindex && (*p == startindex || *p == endindex))
                    break;
                }
            return p - begin;
            }

        size_t varname(const char* begin, const char* end,
                       const char_class_t nameclass)
            {
            const char* p;

            for (p = begin; p != end && nameclass[(int) *p]; p++)
                ;
            return p - begin;
            }

        size_t number(const char* begin, const char* end)
            {
            const char* p;

            for (p = begin; p != end && isdigit((int)*p); p++)
                ;
            return p - begin;
            }

        static size_t substext(const char* begin, const char* end,
                               const var_config_t* config)
            {
            const char* p;

            for (p = begin; p != end && *p != config->varinit && *p != '/'; p++)
                {
                if (*p == config->escape)
                    {
                    if (p + 1 == end)
                        throw incomplete_quoted_pair();
                    p++;
                    }
                }
            return p - begin;
            }

        size_t exptext(const char* begin, const char* end,
                       const var_config_t* config)
            {
            const char* p;

            for (p = begin;
                 p != end
                     && *p != config->varinit
                     && *p != config->enddelim
                     && *p != ':'; p++)
                {
                if (*p == config->escape)
                    {
                    if (p + 1 == end)
                        throw incomplete_quoted_pair();
                    p++;
                    }
                }
            return p - begin;
            }

        size_t exptext_or_variable(const char* begin, const char* end,
                                   const var_config_t* config,
                                   const char_class_t nameclass, var_cb_t lookup,
                                   void* lookup_context,
                                   tokenbuf_t* result, int current_index,
                                   int* rel_lookup_flag)
            {
            const char* p = begin;
            tokenbuf_t tmp;
            size_t rc;

            result->clear();

            if (begin == end)
                return 0;

            do
                {
                rc = exptext(p, end, config);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    result->append(p, rc);
                    p += rc;
                    }

                rc = variable(p, end, config, nameclass, lookup, lookup_context,
                              &tmp, current_index, rel_lookup_flag);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    p += rc;
                    result->append(tmp.begin, tmp.end - tmp.begin);
                    }
                }
            while (rc > 0);

            return p - begin;

          error_return:
            result->clear();
            return rc;
            }

        size_t substext_or_variable(const char* begin, const char* end,
                                    const var_config_t* config,
                                    const char_class_t nameclass, var_cb_t lookup,
                                    void* lookup_context,
                                    tokenbuf_t* result, int current_index,
                                    int* rel_lookup_flag)
            {
            const char* p = begin;
            tokenbuf_t tmp;
            size_t rc;

            result->clear();

            if (begin == end)
                return 0;

            do
                {
                rc = substext(p, end, config);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    result->append(p, rc);
                    p += rc;
                    }

                rc = variable(p, end, config, nameclass, lookup, lookup_context,
                              &tmp, current_index, rel_lookup_flag);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    p += rc;
                    result->append(tmp.begin, tmp.end - tmp.begin);
                    }
                }
            while (rc > 0);

            return p - begin;

          error_return:
            result->clear();
            return rc;
            }
        }
    }
