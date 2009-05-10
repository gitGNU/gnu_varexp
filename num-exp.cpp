/*
 * copyright (c) 2002-2007 Peter Simons <simons@cryp.to>
 * Copyright (c) 2001 The OSSP Project (http://www.ossp.org/)
 * Copyright (c) 2001 Cable & Wireless Deutschland (http://www.cw.com/de/)
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

namespace varexp
{
  namespace internal
  {
    size_t parser::num_exp_read_operand(const char* begin, const char* end, int& result)
    {
      const char* p = begin;
      size_t rc;

      if (begin == end)
        throw incomplete_index_spec();

      if (*p == '(')
      {
        rc = num_exp(++p, end, result);
        p += rc;
        if (p == end)
          throw incomplete_index_spec();
        if (*p != ')')
          throw unclosed_bracket_in_index();
        ++p;
      }
      else if (*p == config.varinit)
      {
        string tmp;
        rc = variable(p, end, tmp);
        p += rc;
        rc = num_exp(tmp.c_str(), tmp.c_str() + tmp.size(), result);
      }
      else if (config.current_index && *p == config.current_index)
      {
        p++;
        result = current_index;
        ++rel_lookup_count;
      }
      else if (isdigit(*p))
      {
        unsigned int tmp;
        rc = number(p, end, tmp);
        result = tmp;
        p += rc;
      }
      else if (*p == '+')
      {
        if (end - p > 1 && isdigit(p[1]))
        {
          p++;
          unsigned int tmp;
          rc = number(p, end, tmp);
          result = tmp;
          p += rc;
        }
        else
          throw invalid_char_in_index_spec();
      }
      else if (*p == '-')
      {
        if (end - p > 1 && isdigit(p[1]))
        {
          p++;
          unsigned int tmp;
          rc = number(p, end, tmp);
          result = tmp;
          p += rc;
          result = 0 - result;
        }
        else
          throw invalid_char_in_index_spec();
      }
      else
        throw invalid_char_in_index_spec();

      return p - begin;
    }

    size_t parser::num_exp(const char *begin, const char *end, int& result)
    {
      const char* p = begin;
      char oper;
      int right;
      int rc;

      if (begin == end)
        throw incomplete_index_spec();

      rc = num_exp_read_operand(p, end, result);
      p += rc;

      while (p != end)
      {
        if (*p == '+' || *p == '-')
        {
          oper = *p++;
          rc = num_exp(p, end, right);
          p += rc;
          if (oper == '+')
            result = result + right;
          else
            result = result - right;
        }
        else if (*p == '*' || *p == '/' || *p == '%')
        {
          oper = *p++;
          rc = num_exp_read_operand(p, end, right);
          p += rc;
          if (oper == '*')
          {
            result = result * right;
          }
          else if (oper == '/')
          {
            if (right == 0)
            {
              throw division_by_zero_in_index();
            }
            else
              result = result / right;
          }
          else if (oper == '%')
          {
            if (right == 0)
            {
              throw division_by_zero_in_index();
            }
            else
              result = result % right;
          }
        }
        else
          break;
      }
      return p - begin;
    }
  }
}
