#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t input(const char* begin, const char* end,
                     const var_config_t* config,
                     const char_class_t nameclass, var_cb_t lookup,
                     void* lookup_context,
                     tokenbuf_t* output, int current_index,
                     size_t recursion_level, int* rel_lookup_flag)
            {
            const char* p = begin;
            try
                {
                size_t rc, rc2;
                tokenbuf_t result;
                int start, step, stop, open_end;
                int i;
                int output_backup;
                struct wrapper_context wcon;
                int my_rel_lookup_flag;
                int original_rel_lookup_state;
                int loop_limit_length;

                if (rel_lookup_flag == NULL)
                    {
                    rel_lookup_flag  = &my_rel_lookup_flag;
                    *rel_lookup_flag = 0;
                    }

                do
                    {
                    if (p != end && config->startindex && *p == config->startindex)
                        {
                        original_rel_lookup_state = *rel_lookup_flag;
                        loop_limit_length = -1;
                        wcon.lookup = lookup;
                        wcon.context = lookup_context;
                        wcon.rel_lookup_flag = rel_lookup_flag;
                        p++;
                        start = 0;
                        step  = 1;
                        stop  = 0;
                        open_end = 1;
                        rc = 0;
                        output_backup = 0;
                      re_loop:
                        for (i = start;
                             (open_end  && (loop_limit_length < 0 || *rel_lookup_flag > original_rel_lookup_state)) ||
                                 (!open_end && i <= stop);
                             i += step)
                            {
                            *rel_lookup_flag = original_rel_lookup_state;
                            output_backup = output->end - output->begin;
                            rc = input(p, end, config, nameclass, &lookup_wrapper,
                                       &wcon, output, i, recursion_level+1, rel_lookup_flag);
                            if (p[rc] != config->endindex)
                                {
                                throw unterminated_loop_construct();
                                }
                            if (loop_limit_length < 0)
                                {
                                rc2 = loop_limits(p + rc + 1, end, config, nameclass,
                                                  lookup, lookup_context, &start, &step,
                                                  &stop, &open_end);
                                if (rc2 == 0)
                                    {
                                    loop_limit_length = 0;
                                    }
                                else if (rc2 > 0)
                                    {
                                    loop_limit_length = rc2;
                                    output->end = output->begin + output_backup;
                                    goto re_loop;
                                    }
                                }
                            }
                        if (open_end)
                            output->end = output->begin + output_backup;
                        else
                            *rel_lookup_flag = original_rel_lookup_state;
                        p += rc;
                        p++;
                        p += loop_limit_length;
                        continue;
                        }

                    rc = text(p, end, config->varinit, config->startindex,
                              config->endindex, config->escape);
                    if (rc > 0)
                        {
                        output->append(p, rc);
                        p += rc;
                        continue;
                        }

                    rc = variable(p, end, config, nameclass, lookup,
                                  lookup_context, &result,
                                  current_index, rel_lookup_flag);
                    if (rc > 0)
                        {
                        output->append(result.begin, result.end - result.begin);
                        p += rc;
                        continue;
                        }
                    }
                while (p != end && rc > 0);

                if (recursion_level == 0 && p != end)
                    {
                    throw input_isnt_text_nor_variable();
                    }

                return p - begin;
                }
            catch(error& e)
                {
                e.begin   = begin;
                e.current = p;
                e.end     = end;
                throw;
                }
            }
        }
    }
