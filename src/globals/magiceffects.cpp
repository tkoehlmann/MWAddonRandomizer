#include "magiceffects.hpp"

#include "../iohelpers.hpp"

#include <algorithm>
#include <functional>


std::vector<Magic::Effect> Magic::ReadEffects(RecordCollection records)
{
    std::vector<Effect> result;

    for (std::pair<const std::string, std::shared_ptr<Record>> rs : records)
    {
        std::shared_ptr<Record> &r                 = rs.second;
        auto srs                                   = r->GetSubrecords("INDX");
        std::shared_ptr<std::vector<uint8_t>> data = srs[0]->Data;
        auto id                                    = io::read_dword(data->data());
        auto srs_medt                              = r->GetSubrecords("MEDT");
        std::shared_ptr<std::vector<uint8_t>> medt = srs_medt[0]->Data;

        Effect e;
        e.id          = id;
        e.school      = (School)io::read_dword(medt->data());
        e.basecost    = io::read_float(medt->data(), 4);
        int32_t flags = io::read_dword(medt->data(), 8);
        e.affects_attribute =
            id == (int32_t)Magic::EffectType::DrainAttr || id == (int32_t)Magic::EffectType::DamageAttr ||
            id == (int32_t)Magic::EffectType::RestoreAttr || id == (int32_t)Magic::EffectType::FortifyAttr ||
            id == (int32_t)Magic::EffectType::AbsorbAttr;
        e.affects_skill =
            id == (int32_t)Magic::EffectType::DrainSkill || id == (int32_t)Magic::EffectType::DamageSkill ||
            id == (int32_t)Magic::EffectType::RestoreSkill || id == (int32_t)Magic::EffectType::FortifySkill ||
            id == (int32_t)Magic::EffectType::AbsorbSkill;
        e.spellmaking = flags & 0x200;
        e.enchanting  = flags & 0x400;
        e.negative    = flags & 0x800;
        e.record      = r;

        result.push_back(e);
    }

    std::sort(result.begin(), result.end(), [](Effect &e1, Effect &e2) { return e1.id < e2.id; });
    return result;
}
