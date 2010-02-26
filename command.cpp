/*
 * Copyright (c) 2002-2010 Peter Simons <simons@cryp.to>
 * Copyright (c) 2001 The OSSP Project <http://www.ossp.org/>
 * Copyright (c) 2001 Cable & Wireless Deutschland <http://www.cw.com/de/>
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
