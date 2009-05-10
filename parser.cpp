#include <sys/types.h>
#include <regex.h>
#include "internal.hpp"

namespace varexp
{
  namespace internal
  {
    parser::parser(const config_t& _config, callback_t& _lookup)
      : config(_config), lookup(&_lookup), current_index(0), rel_lookup_count(0)
    {
      // Create the character class from the description in the
      // config structure.

      expand_character_class(config.namechars, nameclass);

      // Make sure that the specials defined in the configuration do
      // not appear in the character name class.

      if (nameclass[(int)config.varinit] ||
         nameclass[(int)config.startdelim] ||
         nameclass[(int)config.enddelim] ||
         nameclass[(int)config.escape])
      {
        throw invalid_configuration();
      }
    }
  }
}
