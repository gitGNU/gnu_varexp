#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        void cut_out_offset(tokenbuf_t* data, tokenbuf_t* number1,
                            tokenbuf_t* number2, int isrange)
            {
            tokenbuf_t res;
            const char* p;
            size_t num1;
            size_t num2;

            num1 = number1->toint();
            num2 = number2->toint();

            /* Determine begin of result string. */

            if ((size_t)(data->end - data->begin) < num1)
                throw offset_out_of_bounds();
            else
                p = data->begin + num1;

            /* If num2 is zero, we copy the rest from there. */

            if (num2 == 0)
                {
                res.append(p, data->end - p);
                }
            else
                {
                /* OK, then use num2. */
                if (isrange)
                    {
                    if ((p + num2) > data->end)
                        throw range_out_of_bounds();
                    res.append(p, num2);
                    }
                else
                    {
                    if (num2 < num1)
                        throw offset_logic();
                    if ((data->begin + num2) > data->end)
                        throw range_out_of_bounds();
                    res.append(p, num2 - num1 + 1);
                    }
                }
            data->shallow_move(&res);
            }
        }
    }
