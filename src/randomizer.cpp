#include "randomizer.hpp"

#include "globals/globals.hpp"
#include "iohelpers.hpp"
#include "randomizers/alchemy.hpp"
#include "randomizers/weapons.hpp"

#include <algorithm>
#include <cfloat>
#include <cinttypes>
#include <cstdint>
#include <cstring>


std::vector<Record *> Randomizer::RandomizeWeapons(std::vector<Record *> records, Settings &settings)
{
    return Weapons::Randomize(records, settings);
}

std::vector<Record *> Randomizer::RandomizeAlchemy(std::vector<Record *> records, Settings &settings,
                                                   std::vector<Magic::Effect> &magic_effects,
                                                   std::vector<Skills::Skill> &skills)
{
    return Alchemy::Randomize(records, settings, magic_effects, skills);
}
