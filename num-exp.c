#include "internal.h"

static int num_exp_read_int(const char **begin, const char *end)
    {
    int num = 0;

    do
        {
        num *= 10;
        num += **begin - '0';
        ++(*begin);
        }
    while (isdigit(**begin) && *begin != end);
    return num;
    }

static int num_exp_read_operand(const char *begin, const char *end, int current_index,
                                int *result, int *rel_lookup_flag,
                                const var_config_t *config,
                                const char_class_t nameclass,
                                var_cb_t lookup, void *lookup_context)
    {
    const char* p = begin;
    tokenbuf_t tmp;
    int rc;

    tokenbuf_init(&tmp);

    if (begin == end)
        return VAR_ERR_INCOMPLETE_INDEX_SPEC;

    if (*p == '(')
        {
        rc = num_exp(++p, end, current_index, result,
                     rel_lookup_flag, config, nameclass, lookup, lookup_context);
        if (rc < 0)
            return rc;
        p += rc;
        if (p == end)
            return VAR_ERR_INCOMPLETE_INDEX_SPEC;
        if (*p != ')')
            return VAR_ERR_UNCLOSED_BRACKET_IN_INDEX;
        ++p;
        }
    else if (*p == config->varinit)
        {
        rc = variable(p, end, config, nameclass, lookup,
                      lookup_context, &tmp, current_index, rel_lookup_flag);
        if (rc < 0)
            return rc;
        p += rc;
        rc = num_exp(tmp.begin, tmp.end, current_index, result,
                     rel_lookup_flag, config, nameclass, lookup, lookup_context);
        tokenbuf_free(&tmp);
        }
    else if (config->current_index && *p == config->current_index)
        {
        p++;
        *result = current_index;
        (*rel_lookup_flag)++;
        }
    else if (isdigit(*p))
        {
        *result = num_exp_read_int(&p, end);
        }
    else if (*p == '+')
        {
        if (end - p > 1 && isdigit(p[1]))
            {
            p++;
            *result = num_exp_read_int(&p, end);
            }
        else
            return VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC;
        }
    else if (*p == '-')
        {
        if (end - p > 1 && isdigit(p[1]))
            {
            p++;
            *result = num_exp_read_int(&p, end);
            *result = 0 - *result;
            }
        else
            return VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC;
        }
    else
        return VAR_ERR_INVALID_CHAR_IN_INDEX_SPEC;

    return p - begin;
    }

int num_exp(const char *begin, const char *end, int current_index,
                   int *result, int *rel_lookup_flag,
                   const var_config_t *config,
                   const char_class_t nameclass,
                   var_cb_t lookup, void *lookup_context)

    {
    const char *p = begin;
    char operator;
    int right;
    int rc;

    if (begin == end)
        return VAR_ERR_INCOMPLETE_INDEX_SPEC;

    rc = num_exp_read_operand(p, end, current_index, result,
                              rel_lookup_flag, config, nameclass,
                              lookup, lookup_context);
    if (rc < 0)
        return rc;
    p += rc;

    while (p != end)
        {
        if (*p == '+' || *p == '-')
            {
            operator = *p++;
            rc = num_exp(p, end, current_index, &right,
                         rel_lookup_flag, config, nameclass, lookup, lookup_context);
            if (rc < 0)
                return rc;
            p += rc;
            if (operator == '+')
                *result = *result + right;
            else
                *result = *result - right;
            }
        else if (*p == '*' || *p == '/' || *p == '%')
            {
            operator = *p++;
            rc = num_exp_read_operand(p, end, current_index, &right,
                                      rel_lookup_flag, config, nameclass, lookup, lookup_context);
            if (rc < 0)
                return rc;
            p += rc;
            if (operator == '*')
                {
                *result = *result * right;
                }
            else if (operator == '/')
                {
                if (right == 0)
                    {
                    return VAR_ERR_DIVISION_BY_ZERO_IN_INDEX;
                    }
                else
                    *result = *result / right;
                }
            else if (operator == '%')
                {
                if (right == 0)
                    {
                    return VAR_ERR_DIVISION_BY_ZERO_IN_INDEX;
                    }
                else
                    *result = *result % right;
                }
            }
        else
            break;
        }
    return p - begin;
    }
