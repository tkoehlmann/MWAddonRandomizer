#include "magiceffects.hpp"

#include "../iohelpers.hpp"

std::unique_ptr<std::unordered_map<int32_t, Record*>> ReadMagicEffects(std::vector<Record *> records)
{
    std::unique_ptr<std::unordered_map<int32_t, Record *>> result =
        std::make_unique<std::unordered_map<int32_t, Record *>>();

    for (Record *r : records)
    {
        std::vector<std::unique_ptr<Subrecord>> srs = r->GetSubrecords("INDX");
        auto id                                     = io::read_dword(srs[0]->GetData());
        result->insert(std::pair(id, r));
    }

    return result;
}
