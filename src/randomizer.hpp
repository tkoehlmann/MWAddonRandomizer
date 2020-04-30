#include "globals/magiceffects.hpp"
#include "globals/skills.hpp"
#include "records.hpp"
#include "settings.hpp"

#include <vector>

// TODO: Make this file into a library interface and move the Randomize* functions and other data into files in the
// randomizers subdirectory!

namespace Randomizer
{
std::vector<Record *> RandomizeWeapons(std::vector<Record *> records, Settings &settings);
std::vector<Record *> RandomizeAlchemy(std::vector<Record *> records, Settings &settings,
                                       std::vector<Magic::Effect> &magic_effects, std::vector<Skills::Skill> &skills);
} // namespace Randomizer
