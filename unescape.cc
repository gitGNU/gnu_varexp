#include "internal.hh"

namespace varexp
    {
    static int isoct(char c)
        {
        if (c >= '0' && c <= '7')
            return 1;
        else
            return 0;
        }

    static void expand_octal(const char** src, char** dst, const char* end)
        {
        unsigned char c;

        if (end - *src < 3)
            throw incomplete_octal();
        if (!isoct(**src) || !isoct((*src)[1]) || !isoct((*src)[2]))
            throw invalid_octal();

        c = **src - '0';
        if (c > 3)
            throw octal_too_large();
        c *= 8;
        ++(*src);

        c += **src - '0';
        c *= 8;
        ++(*src);

        c += **src - '0';

        **dst = (char) c;
        ++(*dst);
        }

    static bool ishex(char c)
        {
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
            return true;
        else
            return false;
        }

    static void expand_simple_hex(const char** src, char** dst,
                                  const char* end)
        {
        unsigned char c = 0;

        if (end - *src < 2)
            throw incomplete_hex();
        if (!ishex(**src) || !ishex((*src)[1]))
            throw invalid_hex();

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

        **dst = (char) c;
        ++(*dst);
        }

    static void expand_grouped_hex(const char** src, char** dst,
                                       const char* end)
        {
        while (*src < end && **src != '}')
            {
            expand_simple_hex(src, dst, end);
            ++(*src);
            }
        if (*src == end)
            throw incomplete_grouped_hex();
        }

    static void expand_hex(const char** src, char** dst, const char* end)
        {
        if (*src == end)
            throw incomplete_hex();
        if (**src == '{')
            {
            ++(*src);
            expand_grouped_hex(src, dst, end);
            }
        else
            expand_simple_hex(src, dst, end);
        }

    void var_unescape(const char* src, size_t len, char* dst,
                          int unescape_all)
        {
        const char* end = src + len;
        while (src < end)
            {
            if (*src == '\\')
                {
                if (++src == end)
                    throw incomplete_named_character();
                switch (*src)
                    {
                    case '\\':
                        if (!unescape_all)
                            {
                            *dst++ = '\\';
                            }
                        *dst++ = '\\';
                        break;
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
                        expand_hex(&src, &dst, end);
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
                        if (end - src >= 3 && isdigit((int)src[1]) && isdigit((int)src[2]))
                            {
                            expand_octal(&src, &dst, end);
                            break;
                            }
                    default:
                        if (!unescape_all)
                            {
                            *dst++ = '\\';
                            }
                        *dst++ = *src;
                    }
                ++src;
                }
            else
                *dst++ = *src++;
            }
        *dst = '\0';
        }
    }
