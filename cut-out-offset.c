#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include "internal.h"

#include <stdio.h>

static size_t tokenbuf2int(tokenbuf* number)
    {
    const char* p;
    size_t num = 0;
    for (p = number->begin; p != number->end; ++p)
        {
        num *= 10;
        num += *p - '0';
        }
    return num;
    }

int cut_out_offset(tokenbuf* data, tokenbuf* number1, tokenbuf* number2, int isrange)
    {
    tokenbuf res;
    const char* p;
    size_t num1 = tokenbuf2int(number1);
    size_t num2 = tokenbuf2int(number2);

    printf("*** Cutting out from offset %d to %d.\n", num1, num2);

    /* Determine begin of result string. */

    if ((data->end - data->begin) < num1)
        return VAR_OFFSET_OUT_OF_BOUNDS;
    else
        p = data->begin + num1;

    /* If num2 is zero, we copy the rest from there. */

    if (num2 == 0)
        {
        if (!assign_to_tokenbuf(&res, p, data->end - p))
            return VAR_OUT_OF_MEMORY;
        }
    else                        /* OK, then use num2. */
        {
        if (isrange)
            {
            if ((p + num2) > data->end)
                return VAR_RANGE_OUT_OF_BOUNDS;
            if (!assign_to_tokenbuf(&res, p, num2))
                return VAR_OUT_OF_MEMORY;
            }
        else
            {
            if (num2 < num1)
                return VAR_OFFSET_LOGIC_ERROR;
            if ((data->begin + num2) > data->end)
                return VAR_RANGE_OUT_OF_BOUNDS;
            if (!assign_to_tokenbuf(&res, p, (data->begin + num2) - p))
                return VAR_OUT_OF_MEMORY;
            }
        }
    free_tokenbuf(data);
    move_tokenbuf(&res, data);
    return VAR_OK;
    }
