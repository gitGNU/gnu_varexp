#include <sys/types.h>
#include <regex.h>
#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        int padding(tokenbuf_t* data, tokenbuf_t* widthstr, tokenbuf_t* fill,
                    char position)
            {
            tokenbuf_t result;
            size_t width = widthstr->toint();
            int i;

            if (fill->begin == fill->end)
                return VAR_ERR_EMPTY_PADDING_FILL_STRING;

            if (position == 'l')
                {
                i = width - (data->end - data->begin);
                if (i > 0)
                    {
                    i = i / (fill->end - fill->begin);
                    while (i > 0)
                        {
                        data->append(fill->begin, fill->end - fill->begin);
                        i--;
                        }
                    i = (width - (data->end - data->begin))
                        % (fill->end - fill->begin);
                    data->append(fill->begin, i);
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
                        result.append(fill->begin, fill->end - fill->begin);
                        i--;
                        }
                    i = (width - (data->end - data->begin))
                        % (fill->end - fill->begin);
                    result.append(fill->begin, i);
                    result.append(data->begin, data->end - data->begin);
                    data->shallow_move(&result);
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
                        result.append(fill->begin, fill->end - fill->begin);
                        i--;
                        }
                    i = ((width - (data->end - data->begin)) / 2)
                        % (fill->end - fill->begin);
                    result.append(fill->begin, i);

                    /* Append the actual data string. */

                    result.append(data->begin, data->end - data->begin);

                    /* Append the suffix. */

                    i = width - (result.end - result.begin);
                    i = i / (fill->end - fill->begin);
                    while (i > 0)
                        {
                        result.append(fill->begin, fill->end - fill->begin);
                        i--;
                        }
                    i = width - (result.end - result.begin);
                    result.append(fill->begin, i);

                    /* Move string from temporary buffer to data buffer. */

                    data->shallow_move(&result);
                    }
                }

            return VAR_OK;
            }
        }
    }
