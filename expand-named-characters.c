#include "internal.h"
#include <assert.h>

static int isoct(char c)
    {
    if (c >= '0' && c <= '7')
        return 1;
    else
        return 0;
    }

static var_rc_t expand_octal(char** read_pos, char** write_pos, const char* end_pos)
    {
    unsigned char c;

    if (end_pos - *read_pos < 3)
        return VAR_INCOMPLETE_OCTAL;
    if (!isoct(**read_pos) || !isoct((*read_pos)[1]) || !isoct((*read_pos)[2]))
        return VAR_INVALID_OCTAL;

    c = **read_pos - '0';
    if (c > 3)
        return VAR_OCTAL_TOO_LARGE;
    c *= 8;
    ++(*read_pos);

    c += **read_pos - '0';
    c *= 8;
    ++(*read_pos);

    c += **read_pos - '0';

    **write_pos = (char)c;
    ++(*write_pos);
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

static var_rc_t expand_simple_hex(char** read_pos, char** write_pos, const char* end_pos)
    {
    unsigned char c;

    if (end_pos - *read_pos < 2)
        return VAR_INCOMPLETE_HEX;
    if (!ishex(**read_pos) || !ishex((*read_pos)[1]))
        return VAR_INVALID_HEX;

    if (**read_pos >= '0' && **read_pos <= '9')
        c = **read_pos - '0';
    else if (c >= 'a' && c <= 'f')
        c = **read_pos - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        c = **read_pos - 'A' + 10;

    c = c << 4;
    ++(*read_pos);

    if (**read_pos >= '0' && **read_pos <= '9')
        c += **read_pos - '0';
    else if (**read_pos >= 'a' && **read_pos <= 'f')
        c += **read_pos - 'a' + 10;
    else if (**read_pos >= 'A' && **read_pos <= 'F')
        c += **read_pos - 'A' + 10;

    **write_pos = (char)c;
    ++(*write_pos);
    return VAR_OK;
    }

static var_rc_t expand_grouped_hex(char** read_pos, char** write_pos, const char* end_pos)
    {
    var_rc_t rc;

    while (*read_pos < end_pos && **read_pos != '}')
        {
        if ((rc = expand_simple_hex(read_pos, write_pos, end_pos)) != 0)
            return rc;
        ++(*read_pos);
        }
    if (*read_pos == end_pos)
        return VAR_INCOMPLETE_GROUPED_HEX;

    return VAR_OK;
    }

static var_rc_t expand_hex(char** read_pos, char** write_pos, const char* end_pos)
    {
    if (*read_pos == end_pos)
        return VAR_INCOMPLETE_HEX;
    if (**read_pos == '{')
        {
        ++(*read_pos);
        return expand_grouped_hex(read_pos, write_pos, end_pos);
        }
    else
        return expand_simple_hex(read_pos, write_pos, end_pos);
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

var_rc_t expand_named_characters(char* buffer, size_t len)
    {
    char*    read_pos  = buffer;
    char*    write_pos = buffer;
    char*    end_pos   = buffer + len;
    var_rc_t rc;

    assert(buffer != NULL);

    while (read_pos < end_pos)
        {
        if (*read_pos == '\\')
            {
            if (++read_pos == end_pos)
                return VAR_INCOMPLETE_NAMED_CHARACTER;
            switch (*read_pos)
                {
                case 'n':
                    *write_pos++ = '\n';
                    break;
                case 't':
                    *write_pos++ = '\t';
                    break;
                case 'r':
                    *write_pos++ = '\r';
                    break;
                case 'x':
                    ++read_pos;
                    if ((rc = expand_hex(&read_pos, &write_pos, end_pos)) != 0)
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
                    if ((rc = expand_octal(&read_pos, &write_pos, end_pos)) != 0)
                        return rc;
                    break;
                default:
                    *write_pos++ = *read_pos;
                }
            ++read_pos;
            }
        else
            *write_pos++ = *read_pos++;
        }
    *write_pos = '\0';
    return VAR_OK;
    }
