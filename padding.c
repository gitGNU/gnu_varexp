#include <sys/types.h>
#include <regex.h>
#include "internal.h"

int padding(tokenbuf* data, tokenbuf* widthstr, tokenbuf* fill, char position)
    {
    tokenbuf result;
    size_t width = tokenbuf2int(widthstr);
    int i;

    printf("Padding data '%s' to width '%d' by filling in '%s' to position '%c'.\n",
           data->begin, width, fill->begin, position);

    if (fill->begin == fill->end)
        return VAR_EMPTY_PADDING_FILL_STRING;

    init_tokenbuf(&result);

    if (position == 'l')
        {
        i = width - (data->end - data->begin);
        if (i > 0)
            {
            printf("Missing %d characters at the end of the data string.\n", i);
            i = i / (fill->end - fill->begin);
            printf("That's %d times the padding string.\n", i);
            while(i > 0)
                {
                if (!append_to_tokenbuf(data, fill->begin, fill->end - fill->begin))
                    return VAR_OUT_OF_MEMORY;
                --i;
                }
            i = (width - (data->end - data->begin)) % (fill->end - fill->begin);
            printf("Plus a remainder of %d characters.\n", i);
            if (!append_to_tokenbuf(data, fill->begin, i))
                return VAR_OUT_OF_MEMORY;
            }
        }
    else if (position == 'r')
        {
        i = width - (data->end - data->begin);
        if (i > 0)
            {
            printf("Missing %d characters at the beginning of the data string.\n", i);
            i = i / (fill->end - fill->begin);
            printf("That's %d times the padding string.\n", i);
            while(i > 0)
                {
                if (!append_to_tokenbuf(&result, fill->begin, fill->end - fill->begin))
                    {
                    free_tokenbuf(&result);
                    return VAR_OUT_OF_MEMORY;
                    }
                --i;
                }
            i = (width - (data->end - data->begin)) % (fill->end - fill->begin);
            printf("Plus a remainder of %d characters.\n", i);
            if (!append_to_tokenbuf(&result, fill->begin, i))
                {
                free_tokenbuf(&result);
                return VAR_OUT_OF_MEMORY;
                }
            if (!append_to_tokenbuf(&result, data->begin, data->end - data->begin))
                {
                free_tokenbuf(&result);
                return VAR_OUT_OF_MEMORY;
                }

            free_tokenbuf(data);
            move_tokenbuf(&result, data);
            }
        }
     else if (position == 'c')
        {
        i = (width - (data->end - data->begin)) / 2;
        if (i > 0)
            {
            /* Create the prefix. */

            printf("Missing %d characters at the beginning of the data string.\n", i);
            i = i / (fill->end - fill->begin);
            printf("That's %d times the padding string.\n", i);
            while(i > 0)
                {
                if (!append_to_tokenbuf(&result, fill->begin, fill->end - fill->begin))
                    {
                    free_tokenbuf(&result);
                    return VAR_OUT_OF_MEMORY;
                    }
                --i;
                }
            i = ((width - (data->end - data->begin)) / 2) % (fill->end - fill->begin);
            printf("Plus a remainder of %d characters.\n", i);
            if (!append_to_tokenbuf(&result, fill->begin, i))
                {
                free_tokenbuf(&result);
                return VAR_OUT_OF_MEMORY;
                }

            /* Append the actual data string. */

            if (!append_to_tokenbuf(&result, data->begin, data->end - data->begin))
                {
                free_tokenbuf(&result);
                return VAR_OUT_OF_MEMORY;
                }

            /* Append the suffix. */

            i = width - (result.end - result.begin);
            printf("Missing %d characters at the end of the data string.\n", i);
            i = i / (fill->end - fill->begin);
            printf("That's %d times the padding string.\n", i);
            while(i > 0)
                {
                if (!append_to_tokenbuf(&result, fill->begin, fill->end - fill->begin))
                    {
                    free_tokenbuf(&result);
                    return VAR_OUT_OF_MEMORY;
                    }
                --i;
                }
            i = width - (result.end - result.begin);
            printf("Plus a remainder of %d characters.\n", i);
            if (!append_to_tokenbuf(&result, fill->begin, i))
                {
                free_tokenbuf(&result);
                return VAR_OUT_OF_MEMORY;
                }

            /* Move string from temporary buffer to data buffer. */

            free_tokenbuf(data);
            move_tokenbuf(&result, data);
            }
        }

    return VAR_OK;
    }
