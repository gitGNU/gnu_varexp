#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        static int num_exp_read_int(const char** begin, const char* end)
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

        static int num_exp_read_operand(const char* begin, const char* end, int current_index,
                                        int* result, int* rel_lookup_flag,
                                        const var_config_t* config,
                                        const char_class_t nameclass,
                                        var_cb_t lookup, void* lookup_context)
            {
            const char* p = begin;
            tokenbuf_t tmp;
            int rc;

            if (begin == end)
                throw incomplete_index_spec();

            if (*p == '(')
                {
                rc = num_exp(++p, end, current_index, result,
                             rel_lookup_flag, config, nameclass, lookup, lookup_context);
                if (rc < 0)
                    return rc;
                p += rc;
                if (p == end)
                    throw incomplete_index_spec();
                if (*p != ')')
                    throw unclosed_bracket_in_index();
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
                tmp.clear();
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
                    throw invalid_char_in_index_spec();
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
                    throw invalid_char_in_index_spec();
                }
            else
                throw invalid_char_in_index_spec();

            return p - begin;
            }

        int num_exp(const char* begin, const char* end, int current_index,
                    int* result, int* rel_lookup_flag,
                    const var_config_t* config,
                    const char_class_t nameclass,
                    var_cb_t lookup, void* lookup_context)
            {
            const char* p = begin;
            char oper;
            int right;
            int rc;

            if (begin == end)
                throw incomplete_index_spec();

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
                    oper = *p++;
                    rc = num_exp(p, end, current_index, &right,
                                 rel_lookup_flag, config, nameclass, lookup, lookup_context);
                    if (rc < 0)
                        return rc;
                    p += rc;
                    if (oper == '+')
                        *result = *result + right;
                    else
                        *result = *result - right;
                    }
                else if (*p == '*' || *p == '/' || *p == '%')
                    {
                    oper = *p++;
                    rc = num_exp_read_operand(p, end, current_index, &right,
                                              rel_lookup_flag, config, nameclass, lookup, lookup_context);
                    if (rc < 0)
                        return rc;
                    p += rc;
                    if (oper == '*')
                        {
                        *result = *result * right;
                        }
                    else if (oper == '/')
                        {
                        if (right == 0)
                            {
                            throw division_by_zero_in_index();
                            }
                        else
                            *result = *result / right;
                        }
                    else if (oper == '%')
                        {
                        if (right == 0)
                            {
                            throw division_by_zero_in_index();
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
        }
    }
