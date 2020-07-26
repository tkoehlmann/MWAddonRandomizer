#include "skills.hpp"

#include "../iohelpers.hpp"

#include <algorithm>
#include <cstring>

std::vector<Skills::Skill> Skills::Get(RecordCollection records)
{
    std::vector<Skill> result;

    for (std::pair<const std::string, std::shared_ptr<Record>> rs : records)
    {
        std::shared_ptr<Record> &r = rs.second;
        auto srs_indx              = r->GetSubrecords("INDX");
        auto srs_skdt              = r->GetSubrecords("SKDT");
        auto srs_desc              = r->GetSubrecords("DESC");
        Skill s;

        s.id          = *srs_indx[0]->Data->data();
        s.description = std::string((char *)srs_desc[0]->Data->data());
        s.skdt_size   = srs_skdt[0]->GetDataSize();
        s.skdt        = std::make_shared<std::vector<uint8_t>>(*srs_skdt[0]->Data);
        s.record      = r;
        result.push_back(s);
    }

    std::sort(result.begin(), result.end(), [](Skill &s1, Skill &s2) { return s1.id < s2.id; });
    return result;
}

Skills::Skill::Skill() {}

Skills::Skill::Skill(const Skill &other)
    : id(other.id), skdt_size(other.skdt_size), description(other.description), record(other.record)
{
    skdt  = std::make_shared<std::vector<uint8_t>>(skdt_size);
    *skdt = *other.skdt;
}

Skills::Skill &Skills::Skill::operator=(const Skill rhs)
{
    id          = rhs.id;
    skdt_size   = rhs.skdt_size;
    description = rhs.description;
    record      = rhs.record;

    skdt  = std::make_shared<std::vector<uint8_t>>(skdt_size);
    *skdt = *rhs.skdt;

    return *this;
}
