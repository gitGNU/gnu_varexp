#include "internal.hpp"
#include <cstdio>

namespace varexp
{
  namespace internal
  {
    size_t parser::command(const char *begin, const char *end, string& result)
    {
      const char* p = begin;
      size_t rc;

      if (begin == end)
        return 0;

      switch (tolower(*p))
      {
        case 'l':                   // Turn data to lowercase.
          for (size_t i = 0; i < result.size(); ++i)
            result[i] = tolower(result[i]);
          ++p;
          break;

        case 'u':                   // Turn data to uppercase.
          for (size_t i = 0; i < result.size(); ++i)
            result[i] = toupper(result[i]);
          ++p;
          break;

        case 'o':                   // Cut out substrings.
          ++p;
          {
            unsigned int num1, num2;
            bool is_range;

            rc = number(p, end, num1);
            if (rc == 0)
              throw missing_start_offset();
            p += rc;

            if (*p == ',')
              is_range = false;
            else if (*p == '-')
              is_range = true;
            else
              throw invalid_offset_delimiter();
            ++p;

            rc = number(p, end, num2);
            if (rc == 0)
              num2 = 0;
            p += rc;

            cut_out_offset(result, num1, num2, is_range);
          }
          break;

        case '#':                   // Substitute length of the string.
          ++p;
          {
            char buf[((sizeof(int)*8)/3)+10];   // sufficient size: <#bits> x log_10(2) + safety
            sprintf(buf, "%d", result.size());
            result.assign(buf);
          }
          break;

        case '-':                   // Substitute parameter if data is empty.
          p++;
          {
            string tmp;
            rc = exptext_or_variable(p, end, tmp);
            if (rc == 0)
              throw missing_parameter_in_command();
            p += rc;
            if (result.empty())
              result = tmp;
          }
          break;

        case '*':                   // Return "" if data is not empty, parameter otherwise.
          p++;
          {
            string tmp;
            rc = exptext_or_variable(p, end, tmp);
            if (rc == 0)
              throw missing_parameter_in_command();
            p += rc;
            if (result.empty())
              result = tmp;
            else
              result.erase();
          }
          break;

        case '+':                   // Substitute parameter if data is not empty.
          p++;
          {
            string tmp;
            rc = exptext_or_variable(p, end, tmp);
            if (rc == 0)
              throw missing_parameter_in_command();
            p += rc;
            if (!result.empty())
            {
              result = tmp;
            }
          }
          break;

        case 's':                   // Search and replace.
          p++;
          {
            if (*p != '/')
              throw malformatted_replace();
            p++;

            string search;
            rc = substext_or_variable(p, end, search);
            p += rc;

            if (*p != '/')
              throw malformatted_replace();
            p++;


            string replace;
            rc = substext_or_variable(p, end, replace);
            p += rc;

            if (*p != '/')
              throw malformatted_replace();
            p++;

            rc = exptext(p, end);
            string flags(p, rc);
            p += rc;

            search_and_replace(result, search, replace, flags);
          }
          break;

        case 'y':                   // Transpose characters from class A to class B.
          p++;
          {
            if (*p != '/')
              throw malformatted_transpose();
            p++;

            string srcclass;
            rc = substext_or_variable(p, end, srcclass);
            p += rc;

            if (*p != '/')
              throw malformatted_transpose();
            p++;

            string dstclass;
            rc = substext_or_variable(p, end, dstclass);
            p += rc;

            if (*p != '/')
              throw malformatted_transpose();
            ++p;

            transpose(result, srcclass, dstclass);
          }
          break;

        case 'p':                   // Padding.
          p++;
          {
            if (*p != '/')
              throw malformatted_padding();
            p++;

            unsigned int width;
            rc = number(p, end, width);
            if (rc == 0)
              throw missing_padding_width();
            p += rc;

            if (*p != '/')
              throw malformatted_padding();
            p++;

            string fillstring;
            rc = substext_or_variable(p, end, fillstring);
            p += rc;

            if (*p != '/')
              throw malformatted_padding();
            p++;

            if (*p != 'l' && *p != 'c' && *p != 'r')
              throw malformatted_padding();
            p++;
            padding(result, width, fillstring, p[-1]);
          }
          break;

        default:
          throw unknown_command_char();
      }

      // Exit gracefully.

      return p - begin;
    }
  }
}
