#include <cinttypes>
#include <cfloat>
#include <cstdint>
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

Weapons::WeaponData::WeaponData(Settings &s, int8_t *global_min, int8_t *global_max, std::vector<int8_t> *global_values) :
    settings(s),
    damage_global_min(global_min),
    damage_global_max(global_max),
    damage_global_values(global_values),
    damage_chop(Weapons::MinMaxData<int8_t>(INT8_MAX, INT8_MIN, damage_global_min, damage_global_max, damage_global_values)),
    damage_slash(Weapons::MinMaxData<int8_t>(INT8_MAX, INT8_MIN, damage_global_min, damage_global_max, damage_global_values)),
    damage_thrust(Weapons::MinMaxData<int8_t>(INT8_MAX, INT8_MIN, damage_global_min, damage_global_max, damage_global_values)),
    weight(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr),
    value(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr),
    health(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr),
    speed(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr),
    enchant(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr),
    resistance(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr)
{
    rng = [this](int i) { return settings.GetNext(i); };
}

void Weapons::WeaponData::Shuffle(Settings &settings)
{
    if (settings.WeaponsWeight != ShuffleType::None)
        std::random_shuffle(weight.Values.begin(), weight.Values.end(), rng);
    if (settings.WeaponsValue != ShuffleType::None)
        std::random_shuffle(value.Values.begin(), value.Values.end(), rng);
    if (settings.WeaponsHealth != ShuffleType::None)
        std::random_shuffle(health.Values.begin(), health.Values.end(), rng);
    if (settings.WeaponsSpeed != ShuffleType::None)
        std::random_shuffle(speed.Values.begin(), speed.Values.end(), rng);
    if (settings.WeaponsEnchantPts != ShuffleType::None)
        std::random_shuffle(enchant.Values.begin(), enchant.Values.end(), rng);
    if (settings.WeaponsDamage != ShuffleType::None)
    {
        std::random_shuffle(damage_chop.Values.begin(), damage_chop.Values.end(), rng);
        std::random_shuffle(damage_slash.Values.begin(), damage_slash.Values.end(), rng);
        std::random_shuffle(damage_thrust.Values.begin(), damage_thrust.Values.end(), rng);
    }
    if (settings.WeaponsResistance != ShuffleType::None)
        std::random_shuffle(resistance.Values.begin(), resistance.Values.end(), rng);
    if (settings.WeaponsModels != ShuffleType::None)
        std::random_shuffle(model_values.begin(), model_values.end(), rng);
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

