/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1999 by CyberSolutions GmbH.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by CyberSolutions GmbH.
 *
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __EXPAND_VARIABLES_HPP__
#define __EXPAND_VARIABLES_HPP__

#include "../RegExp/RegExp.hpp"

class variable_lookup
    {
  public:
    virtual string operator() (const string & var) const = 0;
    };

void expand_variables(string& buffer, const variable_lookup& lookup)
    {
    // The following regular expression is hefty but useful. It
    // matches any sequence of characters that fulfills these rules:
    //   1. It has the format '$(something)'.
    //   2. 'something' consists of one or more characters in the set
    //      [A-Za-z0-9_-].
    //   3. The whole expression is located at the beginning of the
    //      line, or, if it is not, it is preceded by a character not
    //      equal to the backslash.
    // Futhermore, the 'something' part in the brackets is located by
    // the second submatch of the regular expression so that we can
    // conveniently extract it and replace the whole expression later.

    const RegExp exp("(^|[^\\\\])\\$\\(([A-Za-z0-9_-]+)\\).*", REG_EXTENDED);
    regmatch_t   pmatch[3];

    // As long as our expression matches (a.k.a. there are still
    // variables to be expanded) we'll expand the match we found.

    while (regexec(exp, buffer.c_str(), 3, pmatch, 0) == 0)
	{
	// We have found a variable. The submatch in the expression
	// includes exactly the name of the variable. Retrieve it now
	// for the lookup.

	size_t var_start = pmatch[2].rm_so;
	size_t var_len = pmatch[2].rm_eo - pmatch[2].rm_so;
	string varname = buffer.substr(var_start, var_len);

	// Now we want the position of the $(NAME) string.
	// Fortunately, this is simple: Just move the left pointer two
	// bytes back and the right pointer one byte forward.

	var_start -= 2;
	var_len   += 3;

	// Replace the $(VAR) string with the contents of the
	// corresponding variable. The lookup class should throw an
	// exception in case the variable does not exist, we don't
	// care about these details.

	buffer.replace(var_start, var_len, lookup(varname));
	}
    }

#endif // !defined(__EXPAND_VARIABLES_HPP__)
