#include "internal.h"
#include <stdlib.h>

void free_tokenbuf(tokenbuf* buf)
    {
    if (buf->begin != NULL && buf->buffer_size > 0)
        free((char*)buf->begin);
    buf->begin = buf->end = NULL;
    buf->buffer_size = 0;
    }
