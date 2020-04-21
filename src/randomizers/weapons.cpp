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

bool Weapons::prevent_shuffle(Record &rec)
{
    // Things that aren't weapons (!string("name", ".*cast.*") and !string("id", ".*bolt.*"))
    std::string id = std::string((char *)rec.GetSubrecords("NAME")[0]->GetData());
    for (std::string prevent_id : {
             // Morrowind.esm artifacts
             "cleaverstfelms",
             "mace of molag bal_unique",
             "daedric_scourge_unique",
             "warhammer_crusher_unique",
             "sunder",
             "crosierstllothis",
             "staff_hasedoki_unique",
             "staff_magnus_unique",
             "dwarven_hammer_volendrung",
             "ebony_bow_auriel",
             "longbow_shadows_unique",
             "katana_bluebrand_unique",
             "katana_goldbrand_unique",
             "claymore_chrysamere_unique",
             "daedric_crescent_unique",
             "claymore_iceblade_unique",
             "longsword_umbra_unique",
             "dagger_fang_unique",
             "bonebiter_bow_unique",
             "shortbow of sanguine sureflight",
             "iron_arrow_uniq_judgement",
             "fork_horripilation_unique",
             "keening",
             "mehrunes'_razor_unique",
             "spear_mercy_unique",
             // Morrowind.esm uniques
             "conoon_chodala_axe_unique",
             "Karpal's Friend",
             "war_axe_airan_ammu",
             "Wind of Ahaz",
             "cloudcleaver_unique",
             "dwarven axe_soultrap",
             "Stormkiss",
             "widowmaker_unique",
             "ane_teria_mace_unique",
             "daedric_club_tgdc",
             "clutterbane",
             "lightofday_unique",
             "steelstaffancestors_ttsa",
             "banhammer_unique",
             "we_hellfirestaff",
             "herder_crook",
             "ebony_staff_tges",
             "silver_staff_dawn_uniq",
             "silver staff of hunger",
             "staff_of_llevule",
             "ebony_staff_trebonius",
             "daedric warhammer_ttgd",
             "Fury",
             "glass claymore_magebane",
             "claymore_Agustas",
             "we_temreki",
             "azura_star_unique",
             "bonebiter_bow_unique",
             "iron_arrow_uniq_judgement",
             "shortbow of sanguine sureflight",
             "daedric dagger_mtas",
             "Dagger of Judgement",
             "ebony_dagger_mehrunes",
             "dwe_jinksword_curse_Unique",
             "racerbeak",
             "we_shimsil",
             "daedric dagger_soultrap",
             "Airan_Ahhe's_Spirit_Spear_uniq",
             "dwarven halberd_soultrap",
             "ebony spear_hrce_unique",
             "erur_dan_spear_unique",
             "Greed",
             "we_illkurok",
             "we_stormforge",
             // Morrowind.esm random stuff that is bad to shuffle
             "magic_bolt",
             "shield_bolt",
             "shock_bolt",
             "VFX_AlterationBolt",
             "VFX_ConjureBolt",
             "VFX_DefaultBolt",
             "VFX_DestructBolt",
             "VFX_FrostBolt",
             "VFX_IllusionBolt",
             "VFX_MysticismBolt",
             "VFX_PoisonBolt",
             "VFX_RestoreBolt",
             // Tribunal.esm artifacts
             "Mace of Slurring",
             "stendar_hammer_unique",
             "Sword of Almalexia",
             "bladepiece_01",
             "nerevarblade_01",
             "nerevarblade_01_flame",
             "Bipolar Blade",
             // Tribunal.esm uniques
             "Gravedigger",
             "King's_Oath_pc",
             "daedric dagger_bar",
             "glass dagger_symmachus_unique",
             "silver dagger_iryon_unique",
             "silver dagger_othril_unique",
             "silver dagger_rathalas_unique",
             "ebony war axe_elanande",
             "dwarven mace_salandas",
             "silver dagger_droth_unique",
             "silver dagger_droth_unique_a",
             "ebony shortsword_soscean",
             "silver spear_uvenim",
         })
    {
        if (id == prevent_id)
            return true;
    }

    return false;
}
