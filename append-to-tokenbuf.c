#include "internal.h"
#include <stdlib.h>
#include <string.h>

#define VAR_INITIAL_BUFFER_SIZE 1

int append_to_tokenbuf(tokenbuf* output, const char* data, int len)
    {
    char*  new_buffer;
    size_t new_size;

    /* Is the tokenbuffer initialized at all? If not, allocate a
       standard-sized buffer to begin with. */

    if (output->begin == NULL ||
        output->end == NULL ||
        output->buffer_size == 0)
        {
        if ((output->begin = output->end = malloc(VAR_INITIAL_BUFFER_SIZE)) == NULL)
            return 0;
        else
            output->buffer_size = VAR_INITIAL_BUFFER_SIZE;
        }

    /* Does the token fit into the current buffer? If not, realloc a
       larger buffer that fits. */

    if ((output->buffer_size - (output->end - output->begin)) < len)
        {
        new_size = output->buffer_size;
        do
            {
            new_size *= 2;
            }
        while ((new_size - (output->end - output->begin)) < len);
        new_buffer = realloc((char*)output->begin, new_size);
        if (new_buffer == NULL)
            return 0;
        output->end = new_buffer + (output->end - output->begin);
        output->begin = new_buffer;
        output->buffer_size = new_size;
        }

    /* Append the data at the end of the current buffer. */

    memcpy((char*)output->end, data, len);
    output->end += len;
    *((char*)output->end) = '\0';
    return 1;
    }
