/*
 * Copyright (c) 1999-2001 by Peter Simons <simons@ieee.org>.
 * All rights reserved.
 */

// ISO C++ headers.
#include <iostream>
#include <cstdlib>

// My own libraries.
#include "expand-variables.hh"

using namespace std;

class env_lookup : public variable_lookup
    {
  public:
    virtual string operator() (const string & var) const
	{
	char* p = getenv(var.c_str());
	if (p)
	    return p;
	else
	    return "";
	}
    };

int
main(int argc, char ** argv)
    {
    env_lookup  lookup;
    string      buffer("SHELL   = \"$(SHELL)\"\n"
		       "EDITOR  = \"$(EDITOR)\"\n"
		       "USER    = \"$(USER)\"\n"
		       "TERM    = \"$(TERM)\"\n"
		       "UNKNWON = \"$(UNKNOWN)\"\n"
		       "RECURSE = \"$(RECURSE)\"\n");

    putenv("RECURSE=$(SAY_YES)");
    putenv("SAY_YES=Yes!");

    expand_variables(buffer, lookup);
    cout << buffer;

    return 0;
    }
