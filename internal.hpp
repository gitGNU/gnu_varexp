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

#ifndef LIB_VARIABLE_EXPAND_INTERNAL_HH
#define LIB_VARIABLE_EXPAND_INTERNAL_HH

#include <string>
#include <string.h>
#include <cctype>
#include "varexp.hpp"

namespace varexp
{
  using std::string;

  namespace internal
  {
    // Turn character class descriptions into a lookup-array.

    typedef char char_class_t[256]; /* 256 == 2 ^ sizeof(unsigned char)*8 */

    // The parser.

    class parser
    {
    protected:
      friend void varexp::expand(string const & input,
                                string & result, callback_t& lookup,
                                const config_t* config);

      parser(const config_t& _config, callback_t& _lookup);
      size_t input(const char* begin, const char* end, string & result);

    private:
      size_t command(char const * begin, char const * end, string & result);
      size_t expression(char const * begin, char const * end, string & result);
      size_t loop_limits(char const * begin, char const * end, int& start, int& step, int& stop, bool& open_end);
      size_t num_exp_read_operand(const char* begin, const char* end, int& result);
      size_t num_exp(char const * begin, char const * end, int& result);
      size_t number(char const * begin, char const * end, unsigned int& num);
      size_t exptext(char const * begin, char const * end);
      size_t exptext_or_variable(char const * begin, char const * end, string & result);
      size_t substext_or_variable(char const * begin, char const * end, string & result);
      size_t variable(char const * begin, char const * end, string & result);
      size_t varname(char const * begin, char const * end);
      size_t text(char const * begin, char const * end);
      size_t substext(const char* begin, const char* end);

      const config_t&  config;
      char_class_t     nameclass;
      callback_t*      lookup;
      int              current_index;
      int              rel_lookup_count;
    };

    // Helper routines used by the parser.

    void cut_out_offset(string & data, unsigned int num1, unsigned int num2, bool is_range);
    void padding(string & data, unsigned int width, string const & fillstring, char position);
    void search_and_replace(string & data, string const & search, string const & replace, string const & flags);
    void transpose(string & data, string const & srcclass, string const & dstclass);
    void expand_character_class(char const * desc, char_class_t char_class);
  }
}

#endif /* !defined(LIB_VARIABLE_EXPAND_INTERNAL_HH) */
