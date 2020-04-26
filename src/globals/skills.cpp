#include <algorithm>
#include "skills.hpp"

std::vector<Skills::Skill> Skills::Get(std::vector<Record *> records)
{
    std::vector<Skill> result;

    for (Record *r : records)
    {
        auto srs_indx = r->GetSubrecords("INDX");
        auto srs_skdt = r->GetSubrecords("SKDT");
        auto srs_desc = r->GetSubrecords("DESC");
        Skill s;
        s.id          = *(int32_t *)srs_indx[0]->GetData().get();
        s.description = std::string((char *)srs_desc[0]->GetData().get());
        s.skdt        = std::move(srs_skdt[0]->GetData());
        s.record      = r;
        result.push_back(std::move(s));
    }

    std::sort(result.begin(), result.end(), [](Skill &s1, Skill &s2) { return s1.id < s2.id; });
    return result;
}
