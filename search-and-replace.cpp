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

#include <sys/types.h>
#include <regex.h>
#include "internal.hpp"

namespace varexp
{
  namespace internal
  {
    inline void expand_regex_replace(const char* data, const string& replace,
                                    regmatch_t* pmatch, string& expanded)
    {
      const char* p = replace.c_str();
      size_t i;

      while (p != replace.c_str() + replace.size())
      {
        if (*p == '\\')
        {
          if (replace.c_str() + replace.size() - p <= 1)
          {
            throw incomplete_quoted_pair();
          }
          p++;
          if (*p == '\\')
          {
            expanded.append(p, 1);
            ++p;
            continue;
          }
          if (!isdigit((int)*p))
          {
            throw unknown_quoted_pair_in_replace();
          }
          i = *p - '0';
          p++;
          if (pmatch[i].rm_so == -1)
          {
            throw submatch_out_of_range();
          }
          expanded.append(data + pmatch[i].rm_so, pmatch[i].rm_eo - pmatch[i].rm_so);
        }
        else
        {
          expanded.append(p, 1);
          ++p;
        }
      }
    }

    void search_and_replace(std::string& data,
                           const std::string& search,
                           const std::string& replace,
                           const std::string& flags)
    {
      bool case_insensitive = false;
      bool global = false;
      bool no_regex = false;

      if (search.empty())
        throw empty_search_string();

      for (string::const_iterator p = flags.begin(); p != flags.end(); ++p)
      {
        switch (tolower(*p))
        {
          case 'i':
            case_insensitive = true;
            break;
          case 'g':
            global = true;
            break;
          case 't':
            no_regex = true;
            break;
          default:
            throw unknown_replace_flag();
        }
      }

      if (no_regex)
      {
        string tmp;
        for (const char* p = data.data(); p != data.data() + data.size();)
        {
          int rc;
          if (case_insensitive)
            rc = strncasecmp(p, search.data(), search.size());
          else
            rc = strncmp(p, search.data(), search.size());
          if (rc != 0)
          {
            // No match, copy character.
            tmp.append(p, 1);
            ++p;
          }
          else
          {
            tmp.append(replace);
            p += search.size();
            if (!global)
            {
              tmp.append(p, data.data() + data.size() - p);
              break;
            }
          }
        }
        data = tmp;
      }
      else
      {
        struct sentry
        {
          sentry() : preg(0) { }
          ~sentry()          { if (preg) regfree(preg); }
          regex_t* preg;
        } s;
        regex_t preg;
        regmatch_t pmatch[10];
        int regexec_flag;

        // Compile the pattern.

        int rc = regcomp(&preg, search.c_str(), REG_NEWLINE | REG_EXTENDED|((case_insensitive)?REG_ICASE:0));
        if (rc != 0)
        {
          throw invalid_regex_in_replace();
        }
        else
          s.preg = &preg;

        // Match the pattern and create the result string in
        // the tmp buffer.

        string tmp;
        for (const char* p = data.c_str(); p != data.c_str() + data.size(); )
        {
          if (p == data.c_str() || p[-1] == '\n')
            regexec_flag = 0;
          else
            regexec_flag = REG_NOTBOL;
          if (regexec(&preg, p, sizeof(pmatch) / sizeof(regmatch_t), pmatch, regexec_flag) == REG_NOMATCH ||
             p + pmatch[0].rm_so == data.c_str() + data.size())
          {
            tmp.append(p, data.c_str() + data.size() - p);
            break;
          }
          else
          {
            string expanded;
            expand_regex_replace(p, replace, pmatch, expanded);
            tmp.append(p, pmatch[0].rm_so).append(expanded);

            p += pmatch[0].rm_eo;
            if (pmatch[0].rm_eo - pmatch[0].rm_so == 0)
            {
              tmp.append(p, 1);
              ++p;
            }
            if (!global)
            {
              tmp.append(p, data.c_str() + data.size() - p);
              break;
            }
          }
        }
        data = tmp;
      }
    }
  }
}
