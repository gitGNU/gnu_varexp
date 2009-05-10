#include "internal.hpp"

namespace varexp
{
  namespace internal
  {
    inline void expand_range(unsigned char a, unsigned char b, char_class_t char_class)
    {
      do
      {
        char_class[static_cast<int>(a)] = 1;
      }
      while (++a <= b);
    }

    void expand_character_class(const char* _desc, char_class_t char_class)
    {
      const unsigned char* desc = reinterpret_cast<const unsigned char*>(_desc);
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
            throw incorrect_class_spec();
          expand_range(desc[0], desc[2], char_class);
          desc += 3;
        }
        else
        {
          char_class[static_cast<int>(*desc)] = 1;
          ++desc;
        }
      }
    }
  }
}
