#include <sys/types.h>
#include <regex.h>
#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        void padding(string& data, unsigned int width, const string& fillstring, char position)
            {
            size_t i;

            if (fillstring.empty())
                throw empty_padding_fill_string();

            if (position == 'l')
                {
                i = width - data.size();
                if (i > 0)
                    {
                    i = i / fillstring.size();
                    while (i > 0)
                        {
                        data.append(fillstring);
                        i--;
                        }
                    i = (width - (data.size())) % (fillstring.size());
                    data.append(fillstring, 0, i);
                    }
                }
            else if (position == 'r')
                {
                string tmp;
                i = width - data.size();
                if (i > 0)
                    {
                    i = i / fillstring.size();
                    while (i > 0)
                        {
                        tmp.append(fillstring);
                        i--;
                        }
                    i = (width - (data.size())) % (fillstring.size());
                    tmp.append(fillstring, 0, i);
                    data.insert(0, tmp);
                    }
                }
            else if (position == 'c')
                {
                string tmp;
                i = (width - (data.size())) / 2;
                if (i > 0)
                    {
                    // Create the prefix.

                    i = i / (fillstring.size());
                    while (i > 0)
                        {
                        tmp.append(fillstring);
                        i--;
                        }
                    i = (width - data.size()) / 2 % fillstring.size();
                    tmp.append(fillstring, 0, i);

                    // Append the actual data string.

                    tmp += data;

                    // Append the suffix.

                    i = (width - tmp.size()) / fillstring.size();
                    while (i > 0)
                        {
                        tmp.append(fillstring);
                        i--;
                        }
                    i = width - tmp.size();
                    tmp.append(fillstring, 0, i);

                    // Move string from temporary buffer to data buffer.

                    data = tmp;
                    }
                }
            }
        }
    }
