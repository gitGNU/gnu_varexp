#include "internal.h"

var_rc_t input(const char *begin, const char *end,
               const var_config_t *config,
               const char_class_t nameclass, var_cb_t lookup,
               void *lookup_context, int force_expand,
               tokenbuf_t *output, int current_index,
               size_t recursion_level, int *rel_lookup_flag)
{
    const char *p = begin;
    int rc, rc2;
    tokenbuf_t result;
    int start, step, stop, open_end;
    int i;
    int output_backup;
    struct wrapper_context wcon;
    int my_rel_lookup_flag;
    int original_rel_lookup_state;
    int loop_limit_length;

    tokenbuf_init(&result);

    if (rel_lookup_flag == NULL) {
        rel_lookup_flag  = &my_rel_lookup_flag;
        *rel_lookup_flag = 0;
    }

    do {
        if (begin != end && config->startindex && *begin == config->startindex) {
            original_rel_lookup_state = *rel_lookup_flag;
            loop_limit_length = -1;
            wcon.lookup = lookup;
            wcon.context = lookup_context;
            wcon.rel_lookup_flag = rel_lookup_flag;
            begin++;
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
                 i += step) {
                *rel_lookup_flag = original_rel_lookup_state;
                output_backup = output->end - output->begin;
                rc = input(begin, end, config, nameclass, &lookup_wrapper,
                           &wcon, 1, output, i, recursion_level+1, rel_lookup_flag);
                if (rc < 0)
                    goto error_return;
                if (begin[rc] != config->endindex) {
                    rc = VAR_ERR_UNTERMINATED_LOOP_CONSTRUCT;
                    goto error_return;
                }
                if (loop_limit_length < 0)
                    {
                    rc2 = loop_limits(begin + rc + 1, end, config, nameclass,
                                      lookup, lookup_context, &start, &step,
                                      &stop, &open_end);
                    if (rc2 < 0)
                        {
                        goto error_return;
                        }
                    else if (rc2 == 0)
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
            begin += rc;
            begin++;
            begin += loop_limit_length;
            continue;
        }

        rc = text(begin, end, config->varinit, config->startindex,
                  config->endindex, config->escape);
        if (rc > 0) {
            if (!tokenbuf_append(output, begin, rc)) {
                rc = VAR_ERR_OUT_OF_MEMORY;
                goto error_return;
            }
            begin += rc;
            continue;
        } else if (rc < 0)
            goto error_return;

        rc = variable(begin, end, config, nameclass, lookup,
                      lookup_context, force_expand, &result,
                      current_index, rel_lookup_flag);
        if (rc > 0) {
            if (!tokenbuf_append(output, result.begin, result.end - result.begin)) {
                rc = VAR_ERR_OUT_OF_MEMORY;
                goto error_return;
            }
            begin += rc;
            continue;
        }
        if (rc < 0)
            goto error_return;
    } while (begin != end && rc > 0);

    if (recursion_level == 0 && begin != end) {
        rc = VAR_ERR_INPUT_ISNT_TEXT_NOR_VARIABLE;
        goto error_return;
    }

    return begin - p;

  error_return:
    tokenbuf_free(output);
    tokenbuf_free(&result);
    output->begin = p;
    output->end = begin;
    output->buffer_size = 0;
    return rc;
}
