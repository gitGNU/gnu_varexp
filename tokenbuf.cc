#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        /* Routines for manipulation of token buffers. */

#define TOKENBUF_INITIAL_BUFSIZE 1

        void tokenbuf_init(tokenbuf_t* buf)
            {
            buf->begin = NULL;
            buf->end = NULL;
            buf->buffer_size = 0;
            return;
            }

        void tokenbuf_move(tokenbuf_t* src, tokenbuf_t* dst)
            {
            dst->begin = src->begin;
            dst->end = src->end;
            dst->buffer_size = src->buffer_size;
            tokenbuf_init(src);
            return;
            }

        int tokenbuf_assign(tokenbuf_t* buf, const char* data, size_t len)
            {
            char* p;

            if ((p = (char*)malloc(len + 1)) == NULL)
                return 0;
            memcpy(p, data, len);
            buf->begin = p;
            buf->end = p + len;
            buf->buffer_size = len + 1;
            *((char*)(buf->end)) = '\0';
            return 1;
            }

        int tokenbuf_append(tokenbuf_t* output, const char* data, size_t len)
            {
            char* new_buffer;
            size_t new_size;
            char* tmp;

            /* Is the tokenbuffer initialized at all? If not, allocate a
               standard-sized buffer to begin with. */
            if (output->begin == NULL)
                {
                if ((output->begin = output->end = (char*)malloc(TOKENBUF_INITIAL_BUFSIZE)) == NULL)
                    return 0;
                output->buffer_size = TOKENBUF_INITIAL_BUFSIZE;
                }

            /* Does the token contain text, but no buffer has been allocated yet? */
            if (output->buffer_size == 0)
                {
                /* Check whether data borders to output. If, we can append
                   simly by increasing the end pointer. */
                if (output->end == data)
                    {
                    output->end += len;
                    return 1;
                    }
                /* OK, so copy the contents of output into an allocated buffer
                   so that we can append that way. */
                if ((tmp = (char*)malloc(output->end - output->begin + len + 1)) == NULL)
                    return 0;
                memcpy(tmp, output->begin, output->end - output->begin);
                output->buffer_size = output->end - output->begin;
                output->begin = tmp;
                output->end = tmp + output->buffer_size;
                output->buffer_size += len + 1;
                }

            /* Does the token fit into the current buffer? If not, realloc a
               larger buffer that fits. */
            if ((output->buffer_size - (output->end - output->begin)) <= len)
                {
                new_size = output->buffer_size;
                do
                    {
                    new_size *= 2;
                    }
                while ((new_size - (output->end - output->begin)) <= len);
                if ((new_buffer = (char*)realloc((char*)output->begin, new_size)) == NULL)
                    return var_rc_t(0);
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

        void tokenbuf_free(tokenbuf_t* buf)
            {
            if (buf->begin != NULL && buf->buffer_size > 0)
                free((char*)buf->begin);
            buf->begin = buf->end = NULL;
            buf->buffer_size = 0;
            return;
            }

        size_t tokenbuf_toint(tokenbuf_t* number)
            {
            const char* p;
            size_t num;

            num = 0;
            for (p = number->begin; p != number->end; ++p)
                {
                num *= 10;
                num += *p - '0';
                }
            return num;
            }
        }
    }
