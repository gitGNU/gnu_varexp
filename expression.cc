#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        size_t parser::expression(const char *begin, const char *end, std::string& result)
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
