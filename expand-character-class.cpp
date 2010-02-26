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

namespace varexp
{
  namespace internal
  {
    inline void expand_range(unsigned char a, unsigned char b, char_class_t char_class)
    {
      do
      {
        char_class[static_cast<int>(a)] = 1;
      }
      while (++a <= b);
    }

    void expand_character_class(const char* _desc, char_class_t char_class)
    {
      const unsigned char* desc = reinterpret_cast<const unsigned char*>(_desc);
      size_t i;

      /* Clear the char_class array. */

      for (i = 0; i < 256; ++i)
        char_class[i] = 0;

      /* Walk through the char_class description and set the appropriate
         entries in the array. */

      while (*desc != '\0')
      {
        if (desc[1] == '-' && desc[2] != '\0')
        {
          if (desc[0] > desc[2])
            throw incorrect_class_spec();
          expand_range(desc[0], desc[2], char_class);
          desc += 3;
        }
        else
        {
          char_class[static_cast<int>(*desc)] = 1;
          ++desc;
        }
      }
    }
  }
}
