#ifndef __MAGICEFFECTS_HPP
#define __MAGICEFFECTS_HPP

#include "../records.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Magic
{
enum class School
{
    Alteration  = 0,
    Conjuration = 1,
    Destruction = 2,
    Illusion    = 3,
    Mysticism   = 4,
    Restoration = 5
};


enum class EffectType
{
    DrainAttr = 17,
    DrainSkill = 21,
    DamageAttr = 22,
    DamageSkill = 26,
    RestoreAttr = 74,
    RestoreSkill = 78,
    FortifyAttr = 79,
    FortifySkill = 83,
    AbsorbAttr = 85,
    AbsorbSkill = 89,
};

struct Effect
{
    int32_t id;
    bool affects_skill;
    bool affects_attribute;
    School school;
    float basecost;
    bool spellmaking;
    bool enchanting;
    bool negative; // ?
    // Use other stuff?
    Record *record;
};

std::vector<Effect> ReadEffects(std::vector<Record *> records);
} // namespace Magic

#endif /* __MAGICEFFECTS_HPP */
