#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        int cut_out_offset(tokenbuf_t* data, tokenbuf_t* number1,
                           tokenbuf_t* number2, int isrange)
            {
            tokenbuf_t res;
            const char* p;
            size_t num1;
            size_t num2;

            num1 = tokenbuf_toint(number1);
            num2 = tokenbuf_toint(number2);

            /* Determine begin of result string. */

            if ((size_t)(data->end - data->begin) < num1)
                return VAR_ERR_OFFSET_OUT_OF_BOUNDS;
            else
                p = data->begin + num1;

            /* If num2 is zero, we copy the rest from there. */

            if (num2 == 0)
                {
                if (!tokenbuf_assign(&res, p, data->end - p))
                    return VAR_ERR_OUT_OF_MEMORY;
                }
            else
                {
                /* OK, then use num2. */
                if (isrange)
                    {
                    if ((p + num2) > data->end)
                        return VAR_ERR_RANGE_OUT_OF_BOUNDS;
                    if (!tokenbuf_assign(&res, p, num2))
                        return VAR_ERR_OUT_OF_MEMORY;
                    }
                else
                    {
                    if (num2 < num1)
                        return VAR_ERR_OFFSET_LOGIC;
                    if ((data->begin + num2) > data->end)
                        return VAR_ERR_RANGE_OUT_OF_BOUNDS;
                    if (!tokenbuf_assign(&res, p, num2 - num1 + 1))
                        return VAR_ERR_OUT_OF_MEMORY;
                    }
                }
            tokenbuf_free(data);
            tokenbuf_move(&res, data);
            return VAR_OK;
            }
        }
    }
