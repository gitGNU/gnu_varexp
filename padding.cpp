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

#include <sys/types.h>
#include <regex.h>
#include "internal.hpp"

namespace varexp
{
  namespace internal
  {
    void padding(string& data, unsigned int width, const string& fillstring, char position)
    {
      size_t i;

      if (fillstring.empty())
        throw empty_padding_fill_string();

      if (position == 'l')
      {
        i = width - data.size();
        if (i > 0)
        {
          i = i / fillstring.size();
          while (i > 0)
          {
            data.append(fillstring);
            i--;
          }
          i = (width - (data.size())) % (fillstring.size());
          data.append(fillstring, 0, i);
        }
      }
      else if (position == 'r')
      {
        string tmp;
        i = width - data.size();
        if (i > 0)
        {
          i = i / fillstring.size();
          while (i > 0)
          {
            tmp.append(fillstring);
            i--;
          }
          i = (width - (data.size())) % (fillstring.size());
          tmp.append(fillstring, 0, i);
          data.insert(0, tmp);
        }
      }
      else if (position == 'c')
      {
        string tmp;
        i = (width - (data.size())) / 2;
        if (i > 0)
        {
          // Create the prefix.

          i = i / (fillstring.size());
          while (i > 0)
          {
            tmp.append(fillstring);
            i--;
          }
          i = (width - data.size()) / 2 % fillstring.size();
          tmp.append(fillstring, 0, i);

          // Append the actual data string.

          tmp += data;

          // Append the suffix.

          i = (width - tmp.size()) / fillstring.size();
          while (i > 0)
          {
            tmp.append(fillstring);
            i--;
          }
          i = width - tmp.size();
          tmp.append(fillstring, 0, i);

          // Move string from temporary buffer to data buffer.

          data = tmp;
        }
      }
    }
  }
}
