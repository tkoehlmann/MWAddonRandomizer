#ifndef __ALCHEMY_HPP
#define __ALCHEMY_HPP

#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

namespace Alchemy
{
    struct EffectData
    {
        int32_t effect_id;
        int32_t skill;
        int32_t attrib_id;
        size_t effect_count;

        EffectData(int32_t e, int32_t s, int32_t a, size_t c)
        : effect_id(e), skill(s), attrib_id(a), effect_count(c)
        { }
    };

    std::vector<Record *> RandomizeAlchemy(std::vector<Record *> records, Settings &settings,
                                           std::unique_ptr<std::unordered_map<int32_t, Record *>> &magic_effects);
}

#endif /* _ALCHEMY_HPP */
