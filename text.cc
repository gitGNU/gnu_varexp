#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        int text(const char* begin, const char* end, char varinit,
                 char startindex, char endindex, char escape)
            {
            const char* p;

            for (p = begin; p != end; ++p)
                {
                if (*p == escape)
                    {
                    if (++p == end)
                        return VAR_ERR_INCOMPLETE_QUOTED_PAIR;
                    }
                else if (*p == varinit)
                    break;
                else if (startindex && (*p == startindex || *p == endindex))
                    break;
                }
            return p - begin;
            }

        int varname(const char* begin, const char* end,
                    const char_class_t nameclass)
            {
            const char* p;

            for (p = begin; p != end && nameclass[(int) *p]; p++)
                ;
            return p - begin;
            }

        int number(const char* begin, const char* end)
            {
            const char* p;

            for (p = begin; p != end && isdigit((int)*p); p++)
                ;
            return p - begin;
            }

        static int substext(const char* begin, const char* end,
                            const var_config_t* config)
            {
            const char* p;

            for (p = begin; p != end && *p != config->varinit && *p != '/'; p++)
                {
                if (*p == config->escape)
                    {
                    if (p + 1 == end)
                        return VAR_ERR_INCOMPLETE_QUOTED_PAIR;
                    p++;
                    }
                }
            return p - begin;
            }

        int exptext(const char* begin, const char* end,
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
                        return VAR_ERR_INCOMPLETE_QUOTED_PAIR;
                    p++;
                    }
                }
            return p - begin;
            }

        int exptext_or_variable(const char* begin, const char* end,
                                const var_config_t* config,
                                const char_class_t nameclass, var_cb_t lookup,
                                void* lookup_context,
                                tokenbuf_t* result, int current_index,
                                int* rel_lookup_flag)
            {
            const char* p = begin;
            tokenbuf_t tmp;
            int rc;

            tokenbuf_init(result);
            tokenbuf_init(&tmp);

            if (begin == end)
                return 0;

            do
                {
                rc = exptext(p, end, config);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    if (!tokenbuf_append(result, p, rc))
                        {
                        rc = VAR_ERR_OUT_OF_MEMORY;
                        goto error_return;
                        }
                    p += rc;
                    }

                rc = variable(p, end, config, nameclass, lookup, lookup_context,
                              &tmp, current_index, rel_lookup_flag);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    p += rc;
                    if (!tokenbuf_append
                        (result, tmp.begin, tmp.end - tmp.begin))
                        {
                        rc = VAR_ERR_OUT_OF_MEMORY;
                        goto error_return;
                        }
                    }
                }
            while (rc > 0);

            tokenbuf_free(&tmp);
            return p - begin;

          error_return:
            tokenbuf_free(&tmp);
            tokenbuf_free(result);
            return rc;
            }

        int substext_or_variable(const char* begin, const char* end,
                                 const var_config_t* config,
                                 const char_class_t nameclass, var_cb_t lookup,
                                 void* lookup_context,
                                 tokenbuf_t* result, int current_index,
                                 int* rel_lookup_flag)
            {
            const char* p = begin;
            tokenbuf_t tmp;
            int rc;

            tokenbuf_init(result);
            tokenbuf_init(&tmp);

            if (begin == end)
                return 0;

            do
                {
                rc = substext(p, end, config);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    if (!tokenbuf_append(result, p, rc))
                        {
                        rc = VAR_ERR_OUT_OF_MEMORY;
                        goto error_return;
                        }
                    p += rc;
                    }

                rc = variable(p, end, config, nameclass, lookup, lookup_context,
                              &tmp, current_index, rel_lookup_flag);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    p += rc;
                    if (!tokenbuf_append
                        (result, tmp.begin, tmp.end - tmp.begin))
                        {
                        rc = VAR_ERR_OUT_OF_MEMORY;
                        goto error_return;
                        }
                    }
                }
            while (rc > 0);

            tokenbuf_free(&tmp);
            return p - begin;

          error_return:
            tokenbuf_free(&tmp);
            tokenbuf_free(result);
            return rc;
            }
        }
    }
