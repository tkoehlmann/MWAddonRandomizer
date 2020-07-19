#include "weapons.hpp"

#include "../globals/globals.hpp"
#include "../records.hpp"

#include <cfloat>
#include <cinttypes>
#include <cstdint>
#include <unordered_map>
#include <vector>

void Weapons::AdditionalData::init_sr(Record *r, std::string field)
{
    for (std::shared_ptr<Subrecord> sr : r->GetSubrecords(field))
        m_subrecords.push_back(*sr);
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

std::vector<std::reference_wrapper<Subrecord>> Weapons::AdditionalData::GetSubrecords()
{
    std::vector<std::reference_wrapper<Subrecord>> result;
    for (Subrecord &sr : m_subrecords)
        result.push_back(sr);
    return result;
}

Weapons::WeaponData::WeaponData(Settings &s, int8_t *global_min, int8_t *global_max, std::vector<int8_t> *global_values)
    : damage_global_min(global_min),
      damage_global_max(global_max),
      damage_global_values(global_values),
      weight(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr),
      value(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr),
      health(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr),
      speed(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr),
      enchant(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr),
      damage_chop(
          Weapons::MinMaxData<int8_t>(INT8_MAX, INT8_MIN, damage_global_min, damage_global_max, damage_global_values)),
      damage_slash(
          Weapons::MinMaxData<int8_t>(INT8_MAX, INT8_MIN, damage_global_min, damage_global_max, damage_global_values)),
      damage_thrust(
          Weapons::MinMaxData<int8_t>(INT8_MAX, INT8_MIN, damage_global_min, damage_global_max, damage_global_values)),
      resistance(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr),
      settings(s)
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

static std::vector<std::string> prevent_shuffle_ids = {
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
    std::string id = rec.Name;
    bool found     = std::find(prevent_shuffle_ids.begin(), prevent_shuffle_ids.end(), id) != prevent_shuffle_ids.end();
    return found;
}

/**
 * TODO:
 * - Shuffling arrows and bolts among each other kind of works, bows used like crossbows have some weird
 *   animation and the bolt/arrow is fired as if by some psychic force. Pretty funny. Not sure if this
 *   needs to be changed or will stay in for the memes.
 */
std::vector<Record *> Weapons::Randomize(std::vector<Record *> records, Settings &settings)
{
    const size_t offset_weight = 0;  // float
    const size_t offset_value  = 4;  // long
    const size_t offset_type   = 8;  // short
    const size_t offset_health = 10; // short
    const size_t offset_speed  = 12; // float
    const size_t offset_reach  = 16; // float
    UNUSED(offset_reach);
    const size_t offset_enchant         = 20; // short
    const size_t offset_chop_min        = 22; // byte
    const size_t offset_chop_max        = 23; // byte
    const size_t offset_slash_min       = 24; // byte
    const size_t offset_slash_max       = 25; // byte
    const size_t offset_thrust_min      = 26; // byte
    const size_t offset_thrust_max      = 27; // byte
    const size_t offset_resistance_flag = 28; // long

    const uint16_t type_ShortBladeOneHand = 0;
    const uint16_t type_LongBladeOneHand  = 1;
    const uint16_t type_LongBladeTwoClose = 2;
    const uint16_t type_BluntOneHand      = 3;
    const uint16_t type_BluntTwoClose     = 4;
    const uint16_t type_BluntTwoWide      = 5;
    const uint16_t type_SpearTwoWide      = 6;
    const uint16_t type_AxeOneHand        = 7;
    const uint16_t type_AxeTwoHand        = 8;
    const uint16_t type_MarksmanBow       = 9;
    const uint16_t type_MarksmanCrossbow  = 10;
    const uint16_t type_MarksmanThrown    = 11;
    const uint16_t type_Arrow             = 12;
    const uint16_t type_Bolt              = 13;


    int8_t damage_melee_global_min;
    int8_t damage_melee_global_max;
    std::vector<int8_t> damage_melee_global_values;
    Weapons::WeaponData melee(settings, &damage_melee_global_min, &damage_melee_global_max,
                              &damage_melee_global_values);
    Weapons::WeaponData bows(settings, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::WeaponData ammo(settings, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now
    Weapons::WeaponData thrown(settings, nullptr, nullptr, nullptr);

    std::unordered_map<uint16_t, Weapons::WeaponData *> weapons = { { type_ShortBladeOneHand, &melee },
                                                                    { type_LongBladeOneHand, &melee },
                                                                    { type_LongBladeTwoClose, &melee },
                                                                    { type_BluntOneHand, &melee },
                                                                    { type_BluntTwoClose, &melee },
                                                                    { type_BluntTwoWide, &melee },
                                                                    { type_SpearTwoWide, &melee },
                                                                    { type_AxeOneHand, &melee },
                                                                    { type_AxeTwoHand, &melee },
                                                                    { type_MarksmanBow, &bows },
                                                                    { type_MarksmanCrossbow, &bows },
                                                                    { type_MarksmanThrown, &thrown },
                                                                    { type_Arrow, &ammo },
                                                                    { type_Bolt, &ammo } };

    // Step 1: Fill min/max and value fields
    for (size_t i = 0; i < records.size(); ++i)
    {
        if (Weapons::prevent_shuffle(*records[i]))
            continue;

        std::shared_ptr<Subrecord> wpdt_srs = records[i]->GetSubrecords("WPDT")[0];
        std::vector<uint8_t> wpdt           = *wpdt_srs->Data;

        // for (int asd = 0; asd < 32; ++asd)
        //     printf("wpdt%2d: 0x%" PRIxPTR "\n", asd, (long unsigned int)wpdt[asd]);

        uint16_t type     = io::read_word(wpdt, offset_type);
        float weight      = io::read_float(wpdt, offset_weight);
        int32_t value     = io::read_dword(wpdt, offset_value);
        int16_t health    = io::read_dword(wpdt, offset_health);
        float speed       = io::read_float(wpdt, offset_speed);
        int16_t enchant   = io::read_word(wpdt, offset_enchant);
        int8_t chop_min   = wpdt[offset_chop_min];
        int8_t chop_max   = wpdt[offset_chop_max];
        int8_t slash_min  = wpdt[offset_slash_min];
        int8_t slash_max  = wpdt[offset_slash_max];
        int8_t thrust_min = wpdt[offset_thrust_min];
        int8_t thrust_max = wpdt[offset_thrust_max];

        weapons[type]->weight.Set(weight);
        weapons[type]->value.Set(value);
        weapons[type]->health.Set(health);
        weapons[type]->speed.Set(speed);
        weapons[type]->enchant.Set(enchant);
        weapons[type]->damage_chop.Set(chop_min);
        weapons[type]->damage_chop.Set(chop_max);
        weapons[type]->damage_slash.Set(slash_min);
        weapons[type]->damage_slash.Set(slash_max);
        weapons[type]->damage_thrust.Set(thrust_min);
        weapons[type]->damage_thrust.Set(thrust_max);
        weapons[type]->resistance.Set(io::read_dword(wpdt, offset_resistance_flag));
        weapons[type]->model_values.push_back(Weapons::AdditionalData(records[i]));
        weapons[type]->records.push_back(records[i]);
    }

    // Step 2: Shuffle if necessary
    melee.Shuffle(settings);
    bows.Shuffle(settings);
    thrown.Shuffle(settings);
    ammo.Shuffle(settings);

    // Step 3: iteate over all records and put in shuffled values instead

    std::vector<Weapons::WeaponData *> weapon_type_values = {
        &melee,
        &bows,
        &thrown,
        &ammo,
    };

    for (auto wtvs : weapon_type_values)
    {
        Weapons::WeaponData *weapon_type = wtvs;

        for (size_t i = 0; i < weapon_type->records.size(); i++)
        {
            Record *weap                        = weapon_type->records[i];
            std::shared_ptr<Subrecord> wpdt_srs = records[i]->GetSubrecords("WPDT")[0];
            std::vector<uint8_t> wpdt           = *wpdt_srs->Data;

            weapon_type->weight.Randomize(false, settings, settings.WeaponsWeight, i, offset_weight, 0, wpdt,
                                          io::write_float);
            switch (settings.WeaponsValue)
            {
                case ShuffleType::None:
                    break;

                case ShuffleType::Random:
                case ShuffleType::Random_Chaos:
                case ShuffleType::Shuffled_Different:
                case ShuffleType::Shuffled_Same:
                    io::write_dword(wpdt, weapon_type->value.Values[i], offset_value);
                    break;

                default:
                    break;
            }
            weapon_type->health.Randomize(false, settings, settings.WeaponsHealth, i, offset_health, 0, wpdt,
                                          io::write_word);
            weapon_type->speed.Randomize(false, settings, settings.WeaponsSpeed, i, offset_speed, 0, wpdt,
                                         io::write_float);
            weapon_type->enchant.Randomize(false, settings, settings.WeaponsEnchantPts, i, offset_enchant, 0, wpdt,
                                           io::write_word);
            weapon_type->resistance.Randomize(false, settings, settings.WeaponsResistance, i, offset_resistance_flag, 0,
                                              wpdt, io::write_dword);
            weapon_type->damage_chop.Randomize(true, settings, settings.WeaponsDamage, i, offset_chop_min,
                                               offset_chop_max, wpdt, io::write_byte);
            weapon_type->damage_slash.Randomize(true, settings, settings.WeaponsDamage, i, offset_slash_min,
                                                offset_slash_max, wpdt, io::write_byte);
            weapon_type->damage_thrust.Randomize(true, settings, settings.WeaponsDamage, i, offset_thrust_min,
                                                 offset_thrust_max, wpdt, io::write_byte);

            *wpdt_srs->Data = wpdt;

            if (settings.WeaponsModels != ShuffleType::None)
            {
                weap->ClearSubrecords({ "MODL", "FNAM", "ITEX", "ENAM", "SCRI" });
                std::vector<std::reference_wrapper<Subrecord>> srs = weapon_type->model_values[i].GetSubrecords();
                for (Subrecord &field_sr : srs)
                    weap->AddSubrecord(field_sr);
            }
        }
    }

    std::vector<Record *> result;
    for (auto wtvs : weapon_type_values)
        for (Record *rec : wtvs->records)
            result.push_back(rec);

    return result;
}
