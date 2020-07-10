#include "magiceffects.hpp"

#include "../iohelpers.hpp"

#include <algorithm>
#include <functional>


std::vector<Magic::Effect> Magic::ReadEffects(std::vector<Record *> records)
{
    std::vector<Effect> result;

    for (Record *r : records)
    {
        auto srs                                   = r->GetSubrecords("INDX");
        std::shared_ptr<std::vector<uint8_t>> data = srs[0]->Data;
        auto id                                    = io::read_dword(data->data());
        auto srs_medt                              = r->GetSubrecords("MEDT");
        std::shared_ptr<std::vector<uint8_t>> medt = srs_medt[0]->Data;

        Effect e;
        e.id                = id;
        e.school            = (School)io::read_dword(medt->data());
        e.basecost          = io::read_float(medt->data(), 4);
        int32_t flags       = io::read_dword(medt->data(), 8);
        e.affects_attribute = id == 17 || id == 22 || id == 74 || id == 79 || id == 85; // FIXME: Remove magic constants
        e.affects_skill     = id == 21 || id == 26 || id == 78 || id == 83 || id == 89; // FIXME: Remove magic constants
        e.spellmaking       = flags & 0x200;
        e.enchanting        = flags & 0x400;
        e.negative          = flags & 0x800;
        e.record            = r;

        result.push_back(e);
    }

    std::sort(result.begin(), result.end(), [](Effect &e1, Effect &e2) { return e1.id < e2.id; });
    return result;
}
