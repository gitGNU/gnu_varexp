#include "internal.hh"

static void expand_range(char a, char b, char_class_t char_class)
    {
    do
        {
        char_class[(int)a] = 1;
        }
    while (++a <= b);
    }

var_rc_t expand_character_class(const char* desc, char_class_t char_class)
    {
    size_t i;

    /* Clear the char_class array. */

    for (i = 0; i < 256; ++i)
        char_class[i] = 0;

    /* Walk through the char_class description and set the appropriate
       entries in the array. */

    while (*desc != '\0')
        {
        if (desc[1] == '-' && desc[2] != '\0')
            {
            if (desc[0] > desc[2])
                return VAR_ERR_INCORRECT_CLASS_SPEC;
            expand_range(desc[0], desc[2], char_class);
            desc += 3;
            }
        else
            {
            char_class[(int) *desc] = 1;
            ++desc;
            }
        }

    return VAR_OK;
    }
