#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        int expression(const char* begin, const char* end,
                       const var_config_t* config,
                       const char_class_t nameclass, var_cb_t lookup,
                       void* lookup_context,
                       tokenbuf_t* result, int current_index, int* rel_lookup_flag)
            {
            const char* p = begin;
            const char* data;
            size_t len, buffer_size;
            int rc;
            int idx = 0;
            tokenbuf_t name;
            tokenbuf_t tmp;

            result->clear();

            /* Expect STARTDELIM. */

            if (p == end || *p != config->startdelim)
                return 0;

            if (++p == end)
                throw incomplete_variable_spec();

            /* Get the name of the variable to expand. The name may consist of
               an arbitrary number of VARNAMEs and VARIABLEs. */

            do
                {
                rc = varname(p, end, nameclass);
                if (rc > 0)
                    {
                    name.append(p, rc);
                    p += rc;
                    }

                rc = variable(p, end, config, nameclass, lookup, lookup_context,
                              &tmp, current_index, rel_lookup_flag);
                if (rc < 0)
                    goto error_return;
                if (rc > 0)
                    {
                    name.append(tmp.begin, tmp.end - tmp.begin);
                    p += rc;
                    }
                }
            while (rc > 0);

            /* We must have the complete variable name now, so make sure we
               do. */

            if (name.begin == name.end)
                {
                throw incomplete_variable_spec();
                }

            /* If the next token is START-INDEX, read the index specification. */

            if (config->startindex && *p == config->startindex)
                {
                rc = num_exp(++p, end, current_index, &idx,
                             rel_lookup_flag, config, nameclass, lookup, lookup_context);
                if (rc < 0)
                    goto error_return;
                if (rc == 0)
                    {
                    throw incomplete_index_spec();
                    }
                p += rc;

                if (p == end)
                    {
                    throw incomplete_index_spec();
                    }
                if (*p != config->endindex)
                    {
                    throw invalid_char_in_index_spec();
                    }
                p++;
                }

            /* Now we have the name of the variable stored in "name". The next
               token here must either be an END-DELIM or a ':'. */

            if (p == end || (*p != config->enddelim && *p != ':'))
                {
                throw incomplete_variable_spec();
                }
            p++;

            /* Use the lookup callback to get the variable's contents. */

            rc = (*lookup) (lookup_context, name.begin, name.end - name.begin, idx,
                            &data, &len, &buffer_size);

            /* The preliminary result is the contents of the variable.
               This may be modified by the commands that may follow. */

            result->begin = data;
            result->end = data + len;
            result->buffer_size = buffer_size;

            if (p[-1] == ':')
                {
                /* Parse and execute commands. */

                tmp.clear();
                p--;
                while (p != end && *p == ':')
                    {
                    p++;
                    rc = command(p, end, config, nameclass, lookup,
                                 lookup_context, result,
                                 current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    p += rc;
                    }

                if (p == end || *p != config->enddelim)
                    {
                    throw incomplete_variable_spec();
                    }
                p++;
                }

            /* Exit gracefully. */

            return p - begin;

            /* Exit in case of an error. */

          error_return:
            result->clear();
            return rc;
            }
        }
    }
