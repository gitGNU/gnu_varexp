/*
 * Copyright (c) 2002-2010 Peter Simons <simons@cryp.to>
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
    inline void expand_class_description(const string& src, string& dst)
    {
      for (string::const_iterator p = src.begin(); p != src.end(); )
      {
        if (src.end() - p >= 3 && p[1] == '-')
        {
          if (*p > p[2])
            throw incorrect_transpose_class_spec();
          unsigned c, d;
          for (c = *p, d = p[2]; c <= d; ++c)
          {
            dst.append(1, static_cast<string::value_type>(c));
          }
          p += 3;
        }
        else
        {
          dst.append(1, *p);
          ++p;
        }
      }
    }

    void transpose(string& data, const string& srcdesc, const string& dstdesc)
    {
      string srcclass, dstclass;

      expand_class_description(srcdesc, srcclass);
      expand_class_description(dstdesc, dstclass);

      if (srcclass.empty())
        throw empty_transpose_class();
      if (srcclass.size() != dstclass.size())
        throw transpose_classes_mismatch();

      for (string::iterator p = data.begin(); p != data.end(); ++p)
      {
        for (size_t i = 0; i <= srcclass.size(); ++i)
        {
          if (*p == srcclass[i])
          {
            *p = dstclass[i];
            break;
          }
        }
      }
    }
  }
}
