#include "internal.hh"

namespace varexp
    {
    inline int isoct(char c)
        {
        if (c >= '0' && c <= '7')
            return 1;
        else
            return 0;
        }

    inline void expand_octal(string::const_iterator& src, const string::const_iterator& end, std::string& dst)
        {
        unsigned char c;

        if (end - src < 3)
            throw incomplete_octal();
        if (!isoct(src[0]) || !isoct(src[1]) || !isoct(src[2]))
            throw invalid_octal();

        c = *src - '0';
        if (c > 3)
            throw octal_too_large();
        c *= 8;
        ++src;

        c += *src - '0';
        c *= 8;
        ++src;

        c += *src - '0';

        dst += c;
        }

    inline bool ishex(char c)
        {
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
            return true;
        else
            return false;
        }

    inline void expand_simple_hex(string::const_iterator& src, const string::const_iterator& end, std::string& dst)
        {
        unsigned char c = 0;

        if (end - src < 2)
            throw incomplete_hex();
        if (!ishex(*src) || !ishex(src[1]))
            throw invalid_hex();

        if (*src >= '0' && *src <= '9')
            c = *src - '0';
        else if (c >= 'a' && c <= 'f')
            c = *src - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            c = *src - 'A' + 10;

        c = c << 4;
        ++src;

        if (*src >= '0' && *src <= '9')
            c += *src - '0';
        else if (*src >= 'a' && *src <= 'f')
            c += *src - 'a' + 10;
        else if (*src >= 'A' && *src <= 'F')
            c += *src - 'A' + 10;

        dst += c;
        }

    inline void expand_grouped_hex(string::const_iterator& src, const string::const_iterator& end, std::string& dst)
        {
        while (src != end && *src != '}')
            {
            expand_simple_hex(src, end, dst);
            ++src;
            }
        if (src == end)
            throw incomplete_grouped_hex();
        }

    inline void expand_hex(string::const_iterator& src, const string::const_iterator& end, std::string& dst)
        {
        if (src == end)
            throw incomplete_hex();
        if (*src == '{')
            {
            ++src;
            expand_grouped_hex(src, end, dst);
            }
        else
            expand_simple_hex(src, end, dst);
        }

    void unescape(const string& input, string& output, bool unescape_all)
        {
        string tmp;
        string::const_iterator src = input.begin();
        string::const_iterator end = input.end();

        while (src != end)
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
                            tmp += '\\';
                            }
                        tmp += '\\';
                        break;
                    case 'n':
                        tmp += '\n';
                        break;
                    case 't':
                        tmp += '\t';
                        break;
                    case 'r':
                        tmp += '\r';
                        break;
                    case 'x':
                        ++src;
                        expand_hex(src, end, tmp);
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
                            expand_octal(src, end, tmp);
                            break;
                            }
                    default:
                        if (!unescape_all)
                            {
                            tmp += '\\';
                            }
                        tmp += *src;
                    }
                ++src;
                }
            else
                tmp += *src++;
            }
        output = tmp;
        }
    }
