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
    size_t parser::text(const char *begin, const char *end)
    {
      const char* p;

      for (p = begin; p != end; ++p)
      {
        if (*p == config.escape)
        {
          if (++p == end)
            throw incomplete_quoted_pair();
        }
        else if (*p == config.varinit)
          break;
        else if (config.startindex && (*p == config.startindex || *p == config.endindex))
          break;
      }
      return p - begin;
    }

    size_t parser::varname(const char* begin, const char* end)
    {
      const char* p;

      for (p = begin; p != end && nameclass[(int) *p]; p++)
        ;
      return p - begin;
    }

    size_t parser::number(const char *begin, const char *end, unsigned int& num)
    {
      const char* p = begin;
      num = 0;

      while (isdigit(*p) && p != end)
      {
        num *= 10;
        num += *p - '0';
        ++p;
      }
      return p - begin;
    }

    size_t parser::substext(const char* begin, const char* end)
    {
      const char* p;

      for (p = begin; p != end && *p != config.varinit && *p != '/'; p++)
      {
        if (*p == config.escape)
        {
          if (p + 1 == end)
            throw incomplete_quoted_pair();
          p++;
        }
      }
      return p - begin;
    }

    size_t parser::exptext(const char *begin, const char *end)
    {
      const char* p;

      for (p = begin;
           p != end && *p != config.varinit && *p != config.enddelim && *p != ':';
           ++p)
      {
        if (*p == config.escape)
        {
          if (p + 1 == end)
            throw incomplete_quoted_pair();
          p++;
        }
      }
      return p - begin;
    }

    size_t parser::exptext_or_variable(const char *begin, const char *end, string& result)
    {
      const char* p = begin;
      size_t rc;

      if (begin == end)
        return 0;

      do
      {
        rc = exptext(p, end);
        if (rc > 0)
        {
          result.append(p, rc);
          p += rc;
        }

        string tmp;
        rc = variable(p, end, tmp);
        if (rc > 0)
        {
          result.append(tmp);
          p += rc;
        }
      }
      while (rc > 0);

      return p - begin;
    }

    size_t parser::substext_or_variable(const char *begin, const char *end, string& result)
    {
      const char* p = begin;
      size_t rc;

      if (begin == end)
        return 0;

      do
      {
        rc = substext(p, end);
        if (rc > 0)
        {
          result.append(p, rc);
          p += rc;
        }

        string tmp;
        rc = variable(p, end, tmp);
        if (rc > 0)
        {
          result.append(tmp);
          p += rc;
        }
      }
      while (rc > 0);

      return p - begin;
    }
  }
}
