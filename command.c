#include "internal.h"

int command(const char *begin, const char *end,
                   const var_config_t *config, const char_class_t nameclass,
                   var_cb_t lookup, void *lookup_context, int force_expand,
                   tokenbuf_t *data, int current_index, int *rel_lookup_flag)
{
    const char *p = begin;
    tokenbuf_t tmptokbuf;
    tokenbuf_t search, replace, flags;
    tokenbuf_t number1, number2;
    int isrange;
    int rc;

    tokenbuf_init(&tmptokbuf);
    tokenbuf_init(&search);
    tokenbuf_init(&replace);
    tokenbuf_init(&flags);
    tokenbuf_init(&number1);
    tokenbuf_init(&number2);

    if (begin == end)
        return 0;

    switch (tolower(*p)) {
    case 'l':                   /* Turn data to lowercase. */
        if (data->begin) {
            char *ptr;
            /* If the buffer does not live in an allocated buffer,
               we have to copy it before modifying the contents. */

            if (data->buffer_size == 0) {
                if (!tokenbuf_assign(data, data->begin, data->end - data->begin)) {
                    rc = VAR_ERR_OUT_OF_MEMORY;
                    goto error_return;
                }
            }
            for (ptr = (char *)data->begin; ptr != data->end; ++ptr)
                *ptr = tolower(*ptr);
        }
        p++;
        break;

    case 'u':                   /* Turn data to uppercase. */
        if (data->begin) {
            char *ptr;
            if (data->buffer_size == 0) {
                if (!tokenbuf_assign
                    (data, data->begin, data->end - data->begin)) {
                    rc = VAR_ERR_OUT_OF_MEMORY;
                    goto error_return;
                }
            }
            for (ptr = (char *) data->begin; ptr != data->end; ++ptr)
                *ptr = toupper(*ptr);
        }
        ++p;
        break;

    case 'o':                   /* Cut out substrings. */
        ++p;
        rc = number(p, end);
        if (rc == 0) {
            rc = VAR_ERR_MISSING_START_OFFSET;
            goto error_return;
        }
        number1.begin = p;
        number1.end = p + rc;
        number1.buffer_size = 0;
        p += rc;

        if (*p == ',') {
            isrange = 0;
            ++p;
        } else if (*p == '-') {
            isrange = 1;
            ++p;
        } else {
            rc = VAR_ERR_INVALID_OFFSET_DELIMITER;
            goto error_return;
        }

        rc = number(p, end);
        number2.begin = p;
        number2.end = p + rc;
        number2.buffer_size = 0;
        p += rc;
        if (data->begin) {
            rc = cut_out_offset(data, &number1, &number2, isrange);
            if (rc < 0)
                goto error_return;
        }
        break;

    case '#':                   /* Substitute length of the string. */
        if (data->begin) {
            char buf[((sizeof(int)*8)/3)+10]; /* sufficient size: <#bits> x log_10(2) + safety */
            sprintf(buf, "%d", (int)(data->end - data->begin));
            tokenbuf_free(data);
            if (!tokenbuf_assign(data, buf, strlen(buf))) {
                rc = VAR_ERR_OUT_OF_MEMORY;
                goto error_return;
            }
        }
        ++p;
        break;

    case '-':                   /* Substitute parameter if data is empty. */
        p++;
        rc = exptext_or_variable(p, end, config, nameclass, lookup,
                                 lookup_context, force_expand, &tmptokbuf,
                                 current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        if (rc == 0) {
            rc = VAR_ERR_MISSING_PARAMETER_IN_COMMAND;
            goto error_return;
        }
        p += rc;
        if (data->begin != NULL && data->begin == data->end) {
            tokenbuf_free(data);
            tokenbuf_move(&tmptokbuf, data);
        }
        break;

    case '*':                   /* Return "" if data is not empty, parameter otherwise. */
        p++;
        rc = exptext_or_variable(p, end, config, nameclass, lookup,
                                 lookup_context, force_expand, &tmptokbuf, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        if (rc == 0) {
            rc = VAR_ERR_MISSING_PARAMETER_IN_COMMAND;
            goto error_return;
        }
        p += rc;
        if (data->begin != NULL) {
            if (data->begin == data->end) {
                tokenbuf_free(data);
                tokenbuf_move(&tmptokbuf, data);
            } else {
                tokenbuf_free(data);
                data->begin = data->end = "";
                data->buffer_size = 0;
            }
        }
        break;

    case '+':                   /* Substitute parameter if data is not empty. */
        p++;
        rc = exptext_or_variable(p, end, config, nameclass, lookup,
                                 lookup_context, force_expand, &tmptokbuf, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        if (rc == 0) {
            rc = VAR_ERR_MISSING_PARAMETER_IN_COMMAND;
            goto error_return;
        }
        p += rc;
        if (data->begin != NULL && data->begin != data->end) {
            tokenbuf_free(data);
            tokenbuf_move(&tmptokbuf, data);
        }
        break;

    case 's':                   /* Search and replace. */
        p++;

        if (*p != '/')
            return VAR_ERR_MALFORMATTED_REPLACE;
        p++;

        rc = substext_or_variable(p, end, config, nameclass, lookup,
                                  lookup_context, force_expand, &search, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        p += rc;

        if (*p != '/') {
            rc = VAR_ERR_MALFORMATTED_REPLACE;
            goto error_return;
        }
        p++;

        rc = substext_or_variable(p, end, config, nameclass, lookup,
                                  lookup_context, force_expand, &replace, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        p += rc;

        if (*p != '/') {
            rc = VAR_ERR_MALFORMATTED_REPLACE;
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

        if (data->begin) {
            rc = search_and_replace(data, &search, &replace, &flags);
            if (rc < 0)
                goto error_return;
        }
        break;

    case 'y':                   /* Transpose characters from class A to class B. */
        p++;

        if (*p != '/')
            return VAR_ERR_MALFORMATTED_TRANSPOSE;
        p++;

        rc = substext_or_variable(p, end, config, nameclass, lookup,
                                  lookup_context, force_expand, &search, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        p += rc;

        if (*p != '/') {
            rc = VAR_ERR_MALFORMATTED_TRANSPOSE;
            goto error_return;
        }
        p++;

        rc = substext_or_variable(p, end, config, nameclass, lookup,
                                  lookup_context, force_expand, &replace, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        p += rc;

        if (*p != '/') {
            rc = VAR_ERR_MALFORMATTED_TRANSPOSE;
            goto error_return;
        } else
            ++p;

        if (data->begin) {
            rc = transpose(data, &search, &replace);
            if (rc < 0)
                goto error_return;
        }
        break;


    case 'p':                   /* Padding. */
        p++;

        if (*p != '/')
            return VAR_ERR_MALFORMATTED_PADDING;
        p++;

        rc = number(p, end);
        if (rc == 0) {
            rc = VAR_ERR_MISSING_PADDING_WIDTH;
            goto error_return;
        }
        number1.begin = p;
        number1.end = p + rc;
        number1.buffer_size = 0;
        p += rc;

        if (*p != '/') {
            rc = VAR_ERR_MALFORMATTED_PADDING;
            goto error_return;
        }
        p++;

        rc = substext_or_variable(p, end, config, nameclass, lookup,
                                  lookup_context, force_expand, &replace, current_index, rel_lookup_flag);
        if (rc < 0)
            goto error_return;
        p += rc;

        if (*p != '/') {
            rc = VAR_ERR_MALFORMATTED_PADDING;
            goto error_return;
        }
        p++;

        if (*p != 'l' && *p != 'c' && *p != 'r') {
            rc = VAR_ERR_MALFORMATTED_PADDING;
            goto error_return;
        }
        p++;

        if (data->begin) {
            rc = padding(data, &number1, &replace, p[-1]);
            if (rc < 0)
                goto error_return;
        }
        break;

    default:
        return VAR_ERR_UNKNOWN_COMMAND_CHAR;
    }

    /* Exit gracefully. */

    tokenbuf_free(&tmptokbuf);
    tokenbuf_free(&search);
    tokenbuf_free(&replace);
    tokenbuf_free(&flags);
    tokenbuf_free(&number1);
    tokenbuf_free(&number2);
    return p - begin;

  error_return:
    tokenbuf_free(data);
    tokenbuf_free(&tmptokbuf);
    tokenbuf_free(&search);
    tokenbuf_free(&replace);
    tokenbuf_free(&flags);
    tokenbuf_free(&number1);
    tokenbuf_free(&number2);
    return rc;
}
