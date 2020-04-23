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

static std::vector<std::string> prevent_shuffle_ids =
{
    // Morrowind.esm artifacts
    "cleaverstfelms",
    "mace of molag bal_unique",
    "mace of molag bal_unique_x",
    "daedric_scourge_unique",
    "warhammer_crusher_unique",
    "warhammer_crusher_unique_x",
    "sunder",
    "crosierstllothis",
    "staff_hasedoki_unique",
    "staff_hasedoki_unique_x",
    "staff_magnus_unique",
    "staff_magnus_unique_x",
    "dwarven_hammer_volendrung",
    "ebony_bow_auriel",
    "ebony_bow_auriel_X",
    "longbow_shadows_unique",
    "longbow_shadows_unique_x",
    "katana_bluebrand_unique",
    "katana_goldbrand_unique",
    "katana_goldbrand_unique_x",
    "claymore_chrysamere_unique",
    "claymore_chrysamere_unique_x",
    "daedric_crescent_unique",
    "claymore_iceblade_unique",
    "claymore_iceblade_unique_x",
    "longsword_umbra_unique",
    "longsword_umbra_unique_x",
    "dagger_fang_unique",
    "dagger_fang_unique_x",
    "bonebiter_bow_unique",
    "shortbow of sanguine sureflight",
    "iron_arrow_uniq_judgement",
    "fork_horripilation_unique",
    "keening",
    "mehrunes'_razor_unique",
    "spear_mercy_unique",
    "spear_mercy_unique_x",
    // Morrowind.esm uniques and items required for quests
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
    "daedric warhammer_ttgd_x",
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
    "false_sunder",
    "sunder_fake",
    "iron dagger_telasero_unique",
    "we_stormforge",
    "axe_queen_of_bats_unique",
    "lugrub's axe",
    "dwarven war axe_redas",
    "ebony staff caper",
    "Rusty_Dagger_UNIQUE",
    "devil_tanto_tgamg",
    "daedric wakizashi_hhst",
    "glass_dagger_enamor",
    "fork_horripilation_unique",
    "dart_uniq_judgement",
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
    "VFX_Multiple2",
    "VFX_Multiple3",
    "VFX_Multiple4",
    "VFX_Multiple5",
    "VFX_Multiple6",
    "VFX_Multiple7",
    "VFX_Multiple8",
    "VFX_Multiple9",
    // Tribunal.esm artifacts
    "Mace of Slurring",
    "Mace of Slurring_x",
    "stendar_hammer_unique",
    "stendar_hammer_unique_x",
    "Sword of Almalexia",
    "bladepiece_01",
    "nerevarblade_01",
    "nerevarblade_01_flame",
    "Bipolar Blade",
    "Bipolar Blade_x",
    // Tribunal.esm uniques
    "Gravedigger",
    "King's_Oath_pc",
    "King's_Oath",
    "daedric dagger_bar",
    "glass dagger_symmachus_unique",
    "glass dagger_symmachus_unique_x",
    "silver dagger_iryon_unique",
    "silver dagger_othril_unique",
    "silver dagger_rathalas_unique",
    "ebony spear_blessed_unique",
    "ebony war axe_elanande",
    "dwarven mace_salandas",
    "silver dagger_droth_unique",
    "silver dagger_droth_unique_a",
    "ebony shortsword_soscean",
    "silver spear_uvenim",
    // Bloodmoon.esm artifacts
    "BM_Mace_Aevar_UNI",
    "BM_hunterspear_unique",
    // Bloodmoon.esm uniques and items required for quests
    "bm nordic pick",
    "steel arrow_Carnius",
    "steel longbow_carnius",
    "steel saber_elberoth",
    "imperial shortsword severio",
    "BM silver dagger wolfender",
    "BM_dagger_wolfgiver",
    "bm_ebonyarrow_s",
    "BM nordic silver axe_spurius",
    "bm reaver battle axe",
    "BM_axe_Heartfang_Unique",
    "BM_hunter_battleaxe_unique",
    "BM Nord Leg",
    "Lucky_Break",
    "warhammer_rammekald_unique",
    "bm_ebony_staff_necro",
    "BM_nordic_silver_lgswd_bloodska",
    "BM nordic silver longsword_cft",
    "BM nordic_longsword_tracker",
    "bm_saber_seasplitter",
    "bm_ebonylongsword_s",
    "solvistapp",
    "BM ice longsword_FG_Unique",
    "nordic claymore_stormfang",
    "BM frostgore",
    "steel spear snow prince",
    "silver arrow_thirsk_1",
    "silver arrow_thirsk_2",
    "silver arrow_thirsk_3",
    "silver arrow_thirsk_4",
    "silver arrow_thirsk_5",
    "silver arrow_thirsk_6",
    "silver arrow_thirsk_7",
    "silver arrow_thirsk_8",
    "silver arrow_thirsk_9",
    "silver arrow_thirsk_0",
};

bool Weapons::prevent_shuffle(Record &rec)
{
    // Things that aren't weapons or are uniques or artifacts
    std::string id = rec.GetName();
    bool found = std::find(prevent_shuffle_ids.begin(), prevent_shuffle_ids.end(), id) != prevent_shuffle_ids.end();
    return found;
}
