#include <sys/types.h>
#include <regex.h>
#include "internal.h"

int padding(tokenbuf_t* data, tokenbuf_t* widthstr, tokenbuf_t* fill,
            char position)
    {
    tokenbuf_t result;
    size_t width = tokenbuf_toint(widthstr);
    int i;

    if (fill->begin == fill->end)
        return VAR_ERR_EMPTY_PADDING_FILL_STRING;

    tokenbuf_init(&result);

    if (position == 'l')
        {
        i = width - (data->end - data->begin);
        if (i > 0)
            {
            i = i / (fill->end - fill->begin);
            while (i > 0)
                {
                if (!tokenbuf_append
                    (data, fill->begin, fill->end - fill->begin))
                    return VAR_ERR_OUT_OF_MEMORY;
                i--;
                }
            i = (width - (data->end - data->begin))
                % (fill->end - fill->begin);
            if (!tokenbuf_append(data, fill->begin, i))
                return VAR_ERR_OUT_OF_MEMORY;
            }
        }
    else if (position == 'r')
        {
        i = width - (data->end - data->begin);
        if (i > 0)
            {
            i = i / (fill->end - fill->begin);
            while (i > 0)
                {
                if (!tokenbuf_append
                    (&result, fill->begin, fill->end - fill->begin))
                    {
                    tokenbuf_free(&result);
                    return VAR_ERR_OUT_OF_MEMORY;
                    }
                i--;
                }
            i = (width - (data->end - data->begin))
                % (fill->end - fill->begin);
            if (!tokenbuf_append(&result, fill->begin, i))
                {
                tokenbuf_free(&result);
                return VAR_ERR_OUT_OF_MEMORY;
                }
            if (!tokenbuf_append(&result, data->begin, data->end - data->begin))
                {
                tokenbuf_free(&result);
                return VAR_ERR_OUT_OF_MEMORY;
                }

            tokenbuf_free(data);
            tokenbuf_move(&result, data);
            }
        }
    else if (position == 'c')
        {
        i = (width - (data->end - data->begin)) / 2;
        if (i > 0)
            {
            /* Create the prefix. */

            i = i / (fill->end - fill->begin);
            while (i > 0)
                {
                if (!tokenbuf_append(&result, fill->begin, fill->end - fill->begin))
                    {
                    tokenbuf_free(&result);
                    return VAR_ERR_OUT_OF_MEMORY;
                    }
                i--;
                }
            i = ((width - (data->end - data->begin)) / 2)
                % (fill->end - fill->begin);
            if (!tokenbuf_append(&result, fill->begin, i))
                {
                tokenbuf_free(&result);
                return VAR_ERR_OUT_OF_MEMORY;
                }

            /* Append the actual data string. */

            if (!tokenbuf_append(&result, data->begin, data->end - data->begin))
                {
                tokenbuf_free(&result);
                return VAR_ERR_OUT_OF_MEMORY;
                }

            /* Append the suffix. */

            i = width - (result.end - result.begin);
            i = i / (fill->end - fill->begin);
            while (i > 0)
                {
                if (!tokenbuf_append
                    (&result, fill->begin, fill->end - fill->begin))
                    {
                    tokenbuf_free(&result);
                    return VAR_ERR_OUT_OF_MEMORY;
                    }
                i--;
                }
            i = width - (result.end - result.begin);
            if (!tokenbuf_append(&result, fill->begin, i))
                {
                tokenbuf_free(&result);
                return VAR_ERR_OUT_OF_MEMORY;
                }

            /* Move string from temporary buffer to data buffer. */

            tokenbuf_free(data);
            tokenbuf_move(&result, data);
            }
        }

    return VAR_OK;
    }
