#include "magiceffects.hpp"

#include "../iohelpers.hpp"

#include <algorithm>


std::vector<Magic::Effect> Magic::ReadEffects(std::vector<Record *> records)
{
    std::vector<Effect> result;

    for (Record *r : records)
    {
        std::vector<std::unique_ptr<Subrecord>> srs = r->GetSubrecords("INDX");
        auto data                                   = srs[0]->GetData();
        auto id                                     = io::read_dword(data);
        auto srs_medt                               = r->GetSubrecords("MEDT");
        auto medt                                   = srs_medt[0]->GetData();
        Effect e;
        e.id = id;
        e.school = (School)io::read_dword(medt);
        e.basecost = io::read_float(medt, 4);
        int32_t flags = io::read_dword(medt, 8);
        e.affects_attribute = id == 17 || id == 22 || id == 74 || id == 79 || id == 85;
        e.affects_skill     = id == 21 || id == 26 || id == 78 || id == 83 || id == 89;
        e.spellmaking   = flags & 0x200;
        e.enchanting  = flags & 0x400;
        e.negative    = flags & 0x800;
        e.record      = r;

        result.push_back(e);
    }

    std::sort(result.begin(), result.end(), [](Effect &e1, Effect &e2) { return e1.id < e2.id; });
    return result;
}
