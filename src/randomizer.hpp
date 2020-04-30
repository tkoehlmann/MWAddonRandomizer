#include "globals/magiceffects.hpp"
#include "globals/skills.hpp"
#include "records.hpp"
#include "settings.hpp"

#include <vector>

// TODO: Make this file into a library interface and move the Randomize* functions and other data into files in the
// randomizers subdirectory!

namespace Randomizer
{
size_t Game(std::vector<std::string> &files, Settings &settings);
} // namespace Randomizer
