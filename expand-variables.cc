/*
 * Copyright (c) 1999-2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// My own libraries.
#include "../RegExp/RegExp.hh"
#include "expand-variables.hh"

using namespace std;

variable_lookup::~variable_lookup()
    {
    }

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
