/*
 * Copyright (c) 1999-2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

#ifndef EXPAND_VARIABLES_HH
#define EXPAND_VARIABLES_HH

// ISO C++ headers.
#include <string>

class variable_lookup
    {
  public:
    virtual ~variable_lookup() = 0;
    virtual std::string operator() (const std::string& var) const = 0;
    };

void expand_variables(std::string& buffer, const variable_lookup& lookup);

#endif // !defined(EXPAND_VARIABLES_HH)
