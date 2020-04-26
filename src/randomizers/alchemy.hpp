#ifndef __ALCHEMY_HPP
#define __ALCHEMY_HPP

#include "../globals/magiceffects.hpp"
#include "../globals/skills.hpp"
#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

namespace Alchemy
{
    struct EffectData
    {
        int32_t effect_id;
        int32_t skill_id;
        int32_t attrib_id;
        size_t effect_count;

        EffectData(int32_t e, int32_t s, int32_t a, size_t c)
        : effect_id(e), skill_id(s), attrib_id(a), effect_count(c)
        { }
    };

    std::vector<Record *> Randomize(std::vector<Record *> records, Settings &settings,
                                    std::vector<Magic::Effect> &magic_effects, std::vector<Skills::Skill> &skills);
    } // namespace Alchemy

#endif /* _ALCHEMY_HPP */
