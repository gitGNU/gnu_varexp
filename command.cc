#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t command(const char* begin, const char* end,
                    const var_config_t* config, const char_class_t nameclass,
                    var_cb_t lookup, void* lookup_context,
                    tokenbuf_t* data, int current_index, int* rel_lookup_flag)
            {
            const char* p = begin;
            tokenbuf_t tmptokbuf;
            tokenbuf_t search, replace, flags;
            tokenbuf_t number1, number2;
            int isrange;
            int rc;
            char* ptr;

            if (begin == end)
                return 0;

            switch (tolower(*p))
                {
                case 'l':                   /* Turn data to lowercase. */
                    /* If the buffer does not live in an allocated buffer,
                       we have to copy it before modifying the contents. */

                    data->force_copy();
                    for (ptr = (char*)data->begin; ptr != data->end; ++ptr)
                        *ptr = tolower(*ptr);
                    p++;
                    break;

                case 'u':                   /* Turn data to uppercase. */
                    data->force_copy();
                    for (ptr = (char*) data->begin; ptr != data->end; ++ptr)
                        *ptr = toupper(*ptr);
                    ++p;
                    break;

                case 'o':                   /* Cut out substrings. */
                    ++p;
                    rc = number(p, end);
                    if (rc == 0)
                        {
                        throw missing_start_offset();
                        }
                    number1.begin = p;
                    number1.end = p + rc;
                    number1.buffer_size = 0;
                    p += rc;

                    if (*p == ',')
                        {
                        isrange = 0;
                        ++p;
                        }
                    else
                        if (*p == '-')
                            {
                            isrange = 1;
                            ++p;
                            }
                        else
                            {
                            throw invalid_offset_delimiter();
                            }

                    rc = number(p, end);
                    number2.begin = p;
                    number2.end = p + rc;
                    number2.buffer_size = 0;
                    p += rc;
                    cut_out_offset(data, &number1, &number2, isrange);
                    break;

                case '#':                   /* Substitute length of the string. */
                    char buf[((sizeof(int)*8)/3)+10]; /* sufficient size: <#bits> x log_10(2) + safety */
                    sprintf(buf, "%d", (int)(data->end - data->begin));
                    data->clear();
                    data->append(buf, strlen(buf));
                    ++p;
                    break;

                case '-':                   /* Substitute parameter if data is empty. */
                    p++;
                    rc = exptext_or_variable(p, end, config, nameclass, lookup,
                                             lookup_context, &tmptokbuf,
                                             current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    if (rc == 0)
                        {
                        throw missing_parameter_in_command();
                        }
                    p += rc;
                    if (data->begin != NULL && data->begin == data->end)
                        {
                        data->shallow_move(&tmptokbuf);
                        }
                    break;

                case '*':                   /* Return "" if data is not empty, parameter otherwise. */
                    p++;
                    rc = exptext_or_variable(p, end, config, nameclass, lookup,
                                             lookup_context, &tmptokbuf, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    if (rc == 0)
                        {
                        throw missing_parameter_in_command();
                        }
                    p += rc;
                    if (data->begin == data->end)
                        {
                        data->shallow_move(&tmptokbuf);
                        }
                    else
                        {
                        data->clear();
                        data->begin = data->end = "";
                        data->buffer_size = 0;
                        }
                    break;

                case '+':                   /* Substitute parameter if data is not empty. */
                    p++;
                    rc = exptext_or_variable(p, end, config, nameclass, lookup,
                                             lookup_context, &tmptokbuf, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    if (rc == 0)
                        {
                        throw missing_parameter_in_command();
                        }
                    p += rc;
                    if (data->begin != NULL && data->begin != data->end)
                        {
                        data->shallow_move(&tmptokbuf);
                        }
                    break;

                case 's':                   /* Search and replace. */
                    p++;

                    if (*p != '/')
                        throw malformatted_replace();
                    p++;

                    rc = substext_or_variable(p, end, config, nameclass, lookup,
                                              lookup_context, &search, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    p += rc;

                    if (*p != '/')
                        {
                        throw malformatted_replace();
                        }
                    p++;

                    rc = substext_or_variable(p, end, config, nameclass, lookup,
                                              lookup_context, &replace, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    p += rc;

                    if (*p != '/')
                        {
                        throw malformatted_replace();
                        goto error_return;
                        }
                    p++;

                    rc = exptext(p, end, config);
                    if (rc < 0)
                        goto error_return;
                    flags.begin = p;
                    flags.end = p + rc;
                    flags.buffer_size = 0;
                    p += rc;
                    search_and_replace(data, &search, &replace, &flags);
                    break;

                case 'y':                   /* Transpose characters from class A to class B. */
                    p++;

                    if (*p != '/')
                        throw malformatted_transpose();
                    p++;

                    rc = substext_or_variable(p, end, config, nameclass, lookup,
                                              lookup_context, &search, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    p += rc;

                    if (*p != '/')
                        {
                        throw malformatted_transpose();
                        goto error_return;
                        }
                    p++;

                    rc = substext_or_variable(p, end, config, nameclass, lookup,
                                              lookup_context, &replace, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    p += rc;

                    if (*p != '/')
                        {
                        throw malformatted_transpose();
                        goto error_return;
                        }
                    else
                        ++p;
                    transpose(data, &search, &replace);
                    break;


                case 'p':                   /* Padding. */
                    p++;

                    if (*p != '/')
                        throw malformatted_padding();
                    p++;

                    rc = number(p, end);
                    if (rc == 0)
                        {
                        throw missing_padding_width();
                        }
                    number1.begin = p;
                    number1.end = p + rc;
                    number1.buffer_size = 0;
                    p += rc;

                    if (*p != '/')
                        {
                        throw malformatted_padding();
                        }
                    p++;

                    rc = substext_or_variable(p, end, config, nameclass, lookup,
                                              lookup_context, &replace, current_index, rel_lookup_flag);
                    if (rc < 0)
                        goto error_return;
                    p += rc;

                    if (*p != '/')
                        {
                        throw malformatted_padding();
                        }
                    p++;

                    if (*p != 'l' && *p != 'c' && *p != 'r')
                        {
                        throw malformatted_padding();
                        goto error_return;
                        }
                    p++;
                    padding(data, &number1, &replace, p[-1]);
                    break;

                default:
                    throw unknown_command_char();
                }

            /* Exit gracefully. */

            return p - begin;

          error_return:
            data->clear();
            return rc;
            }
        }
    }
