#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        void cut_out_offset(string& data, unsigned int num1, unsigned int num2, bool is_range)
            {
            if (data.size() < num1)
                throw offset_out_of_bounds();

            if (num2 == 0)
                {
                data.erase(0, num1);
                }
            else
                {
                if (is_range)
                    {
                    if (num1 + num2 > data.size())
                        throw range_out_of_bounds();
                    else
                        data.erase(num1 + num2).erase(0, num1);
                    }
                else
                    {
                    if (num2 < num1)
                        throw offset_logic();
                    else if (num2 > data.size())
                        throw range_out_of_bounds();
                    else
                        data.erase(num2 + 1).erase(0, num1);
                    }
                }
            }
        }
    }

