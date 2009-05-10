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
    size_t parser::loop_limits(const char *begin, const char *end, int& start, int& step, int& stop, bool& open_end)
    {
      const char* p = begin;
      int rc;

      if (begin == end || *p != config.startdelim)
        return 0;
      else
        ++p;

      // Read start value for the loop.

      try
      {
        rc = num_exp(p, end, start);
        p += rc;
      }
      catch(const invalid_char_in_index_spec&)
      {
        start = 0;          // Use default.
      }

      if (*p != ',')
        throw invalid_char_in_loop_limits();
      else
        ++p;

      // Read step value for the loop.

      try
      {
        rc = num_exp(p, end, step);
        p += rc;
      }
      catch(const invalid_char_in_index_spec&)
      {
        step = 1;           // Use default.
        rc = 0;
      }

      if (*p != ',')
      {
        if (*p != config.enddelim)
          throw invalid_char_in_loop_limits();
        else
        {
          ++p;
          stop = step;
          step = 1;
          if (rc > 0)
            open_end = false;
          else
            open_end = true;
          return p - begin;
        }
      }
      else
        ++p;

      // Read stop value for the loop.

      try
      {
        rc = num_exp(p, end, stop);
        open_end = false;
        p += rc;
      }
      catch(const invalid_char_in_index_spec& e)
      {
        stop = 0;           // Use default.
        open_end = true;
      }

      if (*p != config.enddelim)
        throw invalid_char_in_loop_limits();
      else
        ++p;

      return p - begin;
    }
  }
}
