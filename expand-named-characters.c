#include "internal.h"
#include <assert.h>

/* Internal parsing code for octal. */

static int isoct(char c)
    {
    if (c >= '0' && c <= '7')
        return 1;
    else
        return 0;
    }

static var_rc_t expand_octal(const char** src, char** dst, const char* end)
    {
    unsigned char c;

    if (end - *src < 3)
        return VAR_INCOMPLETE_OCTAL;
    if (!isoct(**src) || !isoct((*src)[1]) || !isoct((*src)[2]))
        return VAR_INVALID_OCTAL;

    c = **src - '0';
    if (c > 3)
        return VAR_OCTAL_TOO_LARGE;
    c *= 8;
    ++(*src);

    c += **src - '0';
    c *= 8;
    ++(*src);

    c += **src - '0';

    **dst = (char)c;
    ++(*dst);
    return VAR_OK;
    }

static int ishex(char c)
    {
    if ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F'))
        return 1;
    else
        return 0;
    }

/* Internal parsing code for hex. */

static var_rc_t expand_simple_hex(const char** src, char** dst, const char* end)
    {
    unsigned char c;

    if (end - *src < 2)
        return VAR_INCOMPLETE_HEX;
    if (!ishex(**src) || !ishex((*src)[1]))
        return VAR_INVALID_HEX;

    if (**src >= '0' && **src <= '9')
        c = **src - '0';
    else if (c >= 'a' && c <= 'f')
        c = **src - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        c = **src - 'A' + 10;

    c = c << 4;
    ++(*src);

    if (**src >= '0' && **src <= '9')
        c += **src - '0';
    else if (**src >= 'a' && **src <= 'f')
        c += **src - 'a' + 10;
    else if (**src >= 'A' && **src <= 'F')
        c += **src - 'A' + 10;

    **dst = (char)c;
    ++(*dst);
    return VAR_OK;
    }

static var_rc_t expand_grouped_hex(const char** src, char** dst, const char* end)
    {
    var_rc_t rc;

    while (*src < end && **src != '}')
        {
        if ((rc = expand_simple_hex(src, dst, end)) != 0)
            return rc;
        ++(*src);
        }
    if (*src == end)
        return VAR_INCOMPLETE_GROUPED_HEX;

    return VAR_OK;
    }

static var_rc_t expand_hex(const char** src, char** dst, const char* end)
    {
    if (*src == end)
        return VAR_INCOMPLETE_HEX;
    if (**src == '{')
        {
        ++(*src);
        return expand_grouped_hex(src, dst, end);
        }
    else
        return expand_simple_hex(src, dst, end);
    }

/*
  Expand the following named characters in the buffer:

       \t          tab
       \n          newline
       \r          return
       \033        octal char
       \x1B        hex char
       \x{263a}    wide hex char

  Any other character quoted by a backslash is copied verbatim.
*/

var_rc_t expand_named_characters(const char* src, size_t len, char* dst)
    {
    const char* end = src + len;
    var_rc_t rc;

    assert(src != NULL);
    assert(dst != NULL);

    while (src < end)
        {
        if (*src == '\\')
            {
            if (++src == end)
                return VAR_INCOMPLETE_NAMED_CHARACTER;
            switch (*src)
                {
                case 'n':
                    *dst++ = '\n';
                    break;
                case 't':
                    *dst++ = '\t';
                    break;
                case 'r':
                    *dst++ = '\r';
                    break;
                case 'x':
                    ++src;
                    if ((rc = expand_hex(&src, &dst, end)) != 0)
                        return rc;
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    if ((rc = expand_octal(&src, &dst, end)) != 0)
                        return rc;
                    break;
                default:
                    *dst++ = *src;
                }
            ++src;
            }
        else
            *dst++ = *src++;
        }
    *dst = '\0';
    return VAR_OK;
    }
