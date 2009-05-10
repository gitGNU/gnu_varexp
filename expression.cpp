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
    size_t parser::expression(const char *begin, const char *end, string& result)
    {
      const char* p = begin;
      size_t rc;
      bool have_index;
      int idx;

      // Expect STARTDELIM.

      if (p == end || *p != config.startdelim)
        return 0;

      if (++p == end)
        throw incomplete_variable_spec();

      // Get the name of the variable to expand. The name may
      // consist of an arbitrary number of VARNAMEs and
      // VARIABLEs.

      string name;
      do
      {
        rc = varname(p, end);
        if (rc > 0)
        {
          name.append(p, rc);
          p += rc;
        }

        string tmp;
        rc = variable(p, end, tmp);
        if (rc > 0)
        {
          name.append(tmp);
          p += rc;
        }
      }
      while (rc > 0);

      // We must have the complete variable name now, so make
      // sure we do.

      if (name.empty())
        throw incomplete_variable_spec();

      // If the next token is START-INDEX, read the index
      // specification.

      if (config.startindex && *p == config.startindex)
      {
        int tmp = rel_lookup_count;

        rc = num_exp(++p, end, idx);
        if (rc == 0)
          throw incomplete_index_spec();
        have_index = true;
        p += rc;

        if (p == end)
          throw incomplete_index_spec();
        else if (*p != config.endindex)
          throw invalid_char_in_index_spec();
        else
          p++;

        // Normalize rel_lookup_count.

        if (rel_lookup_count > tmp)
          rel_lookup_count = tmp + 1;
      }
      else
        have_index = false;

      // Now we have the name of the variable stored in "name".
      // The next token here must either be an END-DELIM or a
      // ':'.

      if (p == end || (*p != config.enddelim && *p != ':'))
        throw incomplete_variable_spec();
      else
        p++;

      // Use the lookup callback to get the variable's contents.

      string data;
      if (have_index)
        (*lookup)(name, idx, data);
      else
        (*lookup)(name, data);

      // The preliminary result is the contents of the variable.
      // This may be modified by the commands that may follow.

      if (p[-1] == ':')
      {
        // Parse and execute commands.

        for (--p; p != end && *p == ':'; p += rc)
          rc = command(++p, end, data);

        if (p == end || *p != config.enddelim)
          throw incomplete_variable_spec();

        p++;
      }

      // Exit gracefully.

      result += data;
      return p - begin;
    }
  }
}
