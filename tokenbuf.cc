#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        const size_t tokenbuf_t::INITIAL_BUFSIZE = 1;

        tokenbuf_t::tokenbuf_t() : begin(0), end(0), buffer_size(0)
            {
            }

        tokenbuf_t::~tokenbuf_t()
            {
            clear();
            }

        void tokenbuf_t::clear()
            {
            if (buffer_size > 0)
                free((void*)begin);
            begin       = 0;
            end         = 0;
            buffer_size = 0;
            }

        void tokenbuf_t::force_copy()
            {
            if (buffer_size == 0)
                {
                const char* orig_begin = begin;
                const char* orig_end   = end;
                clear();
                append(orig_begin, orig_end - orig_begin);
                }
            }

        void tokenbuf_t::shallow_move(tokenbuf_t* src)
            {
            if (src != this)
                {
                clear();
                begin             = src->begin;
                end               = src->end;
                buffer_size       = src->buffer_size;
                src->begin        = 0;
                src->end          = 0;
                src->buffer_size  = 0;
                }
            }

        void tokenbuf_t::append(const char* data, size_t len)
            {
            char*  new_buffer;
            size_t new_size;
            char*  tmp;

            /* Is the tokenbuffer initialized at all? If not,
               allocate a standard-sized buffer to begin with. */

            if (begin == 0)
                {
                if ((begin = end = (char*)malloc(INITIAL_BUFSIZE)) == NULL)
                    throw std::bad_alloc();
                buffer_size = INITIAL_BUFSIZE;
                }

            /* Does the token contain text, but no buffer has been
               allocated yet? */

            if (buffer_size == 0)
                {
                /* Check whether data borders to output. If, we
                   can append simly by increasing the end pointer. */

                if (end == data)
                    {
                    end += len;
                    return;
                    }

                /* OK, so copy the contents of output into an
                   allocated buffer so that we can append that
                   way. */

                if ((tmp = (char*)malloc(end - begin + len + 1)) == NULL)
                    throw std::bad_alloc();

                memcpy(tmp, begin, end - begin);
                buffer_size  = end - begin;
                begin        = tmp;
                end          = tmp + buffer_size;
                buffer_size += len + 1;
                }

            /* Does the token fit into the current buffer? If not,
               realloc a larger buffer that fits. */

            if ((buffer_size - (end - begin)) <= len)
                {
                new_size = buffer_size;
                do
                    {
                    new_size *= 2;
                    }
                while ((new_size - (end - begin)) <= len);
                if ((new_buffer = (char*)realloc((char*)begin, new_size)) == NULL)
                    throw std::bad_alloc();
                end = new_buffer + (end - begin);
                begin = new_buffer;
                buffer_size = new_size;
                }

            /* Append the data at the end of the current buffer. */
            memcpy((char*)end, data, len);
            end += len;
            *((char*)end) = '\0';
            }

        unsigned int tokenbuf_t::toint()
            {
            size_t num = 0;
            for (const char* p = begin; p != end; ++p)
                {
                num *= 10;
                num += *p - '0';
                }
            return num;
            }
        }
    }
