#include <unordered_map>
#include <vector>
#include "weapons.hpp"
#include "../records.hpp"

void Weapons::AdditionalData::init_sr(Record *r, std::string field)
{
    if (r->HasSubrecord(field))
    {
        std::vector<std::unique_ptr<Subrecord>> srs = r->GetSubrecords(field);
        m_subrecords.insert(
            m_subrecords.end(),
            std::make_move_iterator(srs.begin()),
            std::make_move_iterator(srs.end()));
    }
}

Weapons::AdditionalData::AdditionalData(Record *r)
{
    // Maybe just dump all subrecords that match these and be done with it?
    init_sr(r, "MODL");
    init_sr(r, "FNAM");
    init_sr(r, "ITEX");
    init_sr(r, "ENAM");
    init_sr(r, "SCRI");
}

std::vector<std::unique_ptr<Subrecord>> Weapons::AdditionalData::GetSubrecords()
{
    std::vector<std::unique_ptr<Subrecord>> result;
    for (auto &sr : m_subrecords)
        result.push_back(std::make_unique<Subrecord>(new Subrecord(*sr)));
    return result;
}

bool Weapons::is_artifact(Record &rec)
{
    //std::string id = std::string((char *)rec["NAME"][0].GetData());
    std::string id = std::string((char *)rec.GetSubrecords("NAME")[0]->GetData());
    return id == "cleaverstfelms" ||
           id == "mace of molag bal_unique" ||
           id == "daedric_scourge_unique" ||
           id == "warhammer_crusher_unique" ||
           //id == "sunder" ||                     -- do we REALLY want to do this?
           id == "crosierstllothis" ||
           id == "staff_hasedoki_unique" ||
           id == "staff_magnus_unique" ||
           id == "dwarven_hammer_volendrung" ||
           id == "ebony_bow_auriel" ||
           id == "longbow_shadows_unique" ||
           id == "katana_bluebrand_unique" ||
           id == "katana_goldbrand_unique" ||
           id == "claymore_chrysamere_unique" ||
           id == "daedric_crescent_unique" ||
           id == "claymore_iceblade_unique" ||
           id == "longsword_umbra_unique" ||
           id == "dagger_fang_unique" ||
           // id == "fork_horripilation_unique" || -- do we REALLY want to do this?
           // id == "keening" ||                   -- do we REALLY want to do this?
           id == "mehrunes'_razor_unique" ||
           id == "spear_mercy_unique";
}

