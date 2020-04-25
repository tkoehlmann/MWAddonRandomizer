#ifndef __MAGICEFFECTS_HPP
#define __MAGICEFFECTS_HPP

#include "../records.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

enum class MagicSchool
{
    Alteration  = 0,
    Conjuration = 1,
    Destruction = 2,
    Illusion    = 3,
    Mysticism   = 4,
    Restoration = 5
};

std::unique_ptr<std::unordered_map<int32_t, Record*>> ReadMagicEffects(std::vector<Record *> records);

#endif /* __MAGICEFFECTS_HPP */
