#include <algorithm>
#include <cstring>
#include <cinttypes>
#include <cstdint>
#include <cfloat>
#include "iohelpers.hpp"
#include "randomizer.hpp"
#include "randomizers/weapons.hpp"


/**
 * TODO:
 * - Shuffling arrows and bolts among each other kind of works, bows used like crossbows have some weird
 *   animation and the bolt/arrow is fired as if by some psychic force. Pretty funny. Not sure if this
 *   needs to be changed or will stay in for the memes.
 */
std::vector<Record*> Randomizer::RandomizeWeapons(std::vector<Record*> records, Settings &settings)
{
    const size_t offset_weight = 0;            // float
    const size_t offset_value = 4;             // long
    const size_t offset_type = 8;              // short
    const size_t offset_health = 10;           // short
    const size_t offset_speed = 12;            // float
    const size_t offset_reach = 16;            // float
    const size_t offset_enchant = 20;          // short
    const size_t offset_chop_min = 22;         // byte
    const size_t offset_chop_max = 23;         // byte
    const size_t offset_slash_min = 24;        // byte
    const size_t offset_slash_max = 25;        // byte
    const size_t offset_thrust_min = 26;       // byte
    const size_t offset_thrust_max = 27;       // byte
    const size_t offset_resistance_flag = 28;  // long

    const uint16_t type_ShortBladeOneHand = 0;
    const uint16_t type_LongBladeOneHand = 1;
    const uint16_t type_LongBladeTwoClose = 2;
    const uint16_t type_BluntOneHand = 3;
    const uint16_t type_BluntTwoClose = 4;
    const uint16_t type_BluntTwoWide = 5;
    const uint16_t type_SpearTwoWide = 6;
    const uint16_t type_AxeOneHand = 7;
    const uint16_t type_AxeTwoHand = 8;
    const uint16_t type_MarksmanBow = 9;
    const uint16_t type_MarksmanCrossbow = 10;
    const uint16_t type_MarksmanThrown = 11;
    const uint16_t type_Arrow = 12;
    const uint16_t type_Bolt = 13;


    int8_t damage_melee_global_min;
    int8_t damage_melee_global_max;
    std::vector<int8_t> damage_melee_global_values;
    Weapons::WeaponData melee(settings, &damage_melee_global_min, &damage_melee_global_max, &damage_melee_global_values);
    Weapons::WeaponData bows(settings, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::WeaponData ammo(settings, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now
    Weapons::WeaponData thrown(settings, nullptr, nullptr, nullptr);

    std::unordered_map<uint16_t, Weapons::WeaponData *> weapons =
    {
        {type_ShortBladeOneHand, &melee},
        {type_LongBladeOneHand, &melee},
        {type_LongBladeTwoClose, &melee},
        {type_BluntOneHand, &melee},
        {type_BluntTwoClose, &melee},
        {type_BluntTwoWide, &melee},
        {type_SpearTwoWide, &melee},
        {type_AxeOneHand, &melee},
        {type_AxeTwoHand, &melee},
        {type_MarksmanBow, &bows},
        {type_MarksmanCrossbow, &bows},
        {type_MarksmanThrown, &thrown},
        {type_Arrow, &ammo},
        {type_Bolt, &ammo}
    };

    // Step 1: Fill min/max and value fields
    for (int i = 0; i < records.size(); ++i)
    {
        if (Weapons::prevent_shuffle(*records[i]))
            continue;

        std::vector<std::unique_ptr<Subrecord>> wpdt_srs = records[i]->GetSubrecords("WPDT");
        uint8_t *wpdt = wpdt_srs[0]->GetData();

        uint16_t type = io::read_word(wpdt + offset_type);
        float weight = io::read_float(wpdt + offset_weight);
        int32_t value = io::read_dword(wpdt + offset_value);
        int16_t health = io::read_dword(wpdt + offset_health);
        float speed = io::read_float(wpdt + offset_speed);
        int16_t enchant = io::read_word(wpdt + offset_enchant);
        int8_t chop_min = wpdt[offset_chop_min];
        int8_t chop_max = wpdt[offset_chop_max];
        int8_t slash_min = wpdt[offset_slash_min];
        int8_t slash_max = wpdt[offset_slash_max];
        int8_t thrust_min = wpdt[offset_thrust_min];
        int8_t thrust_max = wpdt[offset_thrust_max];


        {
            bool ignore = weight == 0;

            std::vector<std::unique_ptr<Subrecord>> fnam_srs = records[i]->GetSubrecords("FNAM");
            if (fnam_srs.size() > 0 && fnam_srs[0]->GetType() == RecordDataType::String && std::string((char*)fnam_srs[0]->GetData()) == "FOR SPELL CASTING")
                ignore = true;
            if (ignore)
            {
                // This will stop randomizing damage values onto and off VFX_DefaultBolt and other nonsense
                continue;
            }
        }

        weapons[type]->weight.Set(weight);
        weapons[type]->value.Set(value);
        weapons[type]->health.Set(health);
        weapons[type]->speed.Set(speed);
        weapons[type]->enchant.Set(enchant);
        weapons[type]->damage_chop.Set(chop_min);
        weapons[type]->damage_chop.Set(chop_max);
        weapons[type]->damage_slash.Set(chop_min);
        weapons[type]->damage_slash.Set(chop_max);
        weapons[type]->damage_thrust.Set(chop_min);
        weapons[type]->damage_thrust.Set(chop_max);
        weapons[type]->resistance.Set(io::read_dword(wpdt + offset_resistance_flag));
        weapons[type]->model_values.push_back(Weapons::AdditionalData(records[i]));
        weapons[type]->records.push_back(records[i]);
    }

    // Step 2: Shuffle if necessary
    melee.Shuffle(settings);
    bows.Shuffle(settings);
    thrown.Shuffle(settings);
    ammo.Shuffle(settings);

    // Step 3: iteate over all records and put in shuffled values instead
    for (auto weapon_type : { &melee, &bows, &thrown, &ammo })
    {
        for (size_t i = 0; i < weapon_type->records.size(); i++)
        {
            Record *weap = weapon_type->records[i];
            std::vector<std::unique_ptr<Subrecord>> wpdt_srs = weap->GetSubrecords("WPDT");
            uint8_t *wpdt = wpdt_srs[0]->GetData();

            weapon_type->weight.Randomize(false, settings, settings.WeaponsWeight, i, offset_weight, 0, wpdt, io::write_float);
            weapon_type->value.Randomize(false, settings, settings.WeaponsValue, i, offset_value, 0, wpdt, io::write_dword, 5000);
            weapon_type->health.Randomize(false, settings, settings.WeaponsHealth, i, offset_health, 0, wpdt, io::write_word);
            weapon_type->speed.Randomize(false, settings, settings.WeaponsSpeed, i, offset_speed, 0, wpdt, io::write_float);
            weapon_type->enchant.Randomize(false, settings, settings.WeaponsEnchantPts, i, offset_enchant, 0, wpdt, io::write_word);
            weapon_type->resistance.Randomize(false, settings, settings.WeaponsResistance, i, offset_resistance_flag, 0, wpdt, io::write_dword);
            weapon_type->damage_chop.Randomize(true, settings, settings.WeaponsDamage, i, offset_chop_min, offset_chop_max, wpdt, io::write_byte, 127);
            weapon_type->damage_slash.Randomize(true, settings, settings.WeaponsDamage, i, offset_slash_min, offset_slash_max, wpdt, io::write_byte, 127);
            weapon_type->damage_thrust.Randomize(true, settings, settings.WeaponsDamage, i, offset_thrust_min, offset_thrust_max, wpdt, io::write_byte, 127);

            weap->ClearSubrecords({"WPDT"});
            wpdt_srs[0]->SetData(wpdt, wpdt_srs[0]->GetDataSize());
            weap->AddSubrecord(std::move(wpdt_srs[0]));

            if (settings.WeaponsModels != ShuffleType::None)
            {
                weap->ClearSubrecords({"MODL", "FNAM", "ITEX", "ENAM", "SCRI"});
                std::vector<std::unique_ptr<Subrecord>> srs = weapon_type->model_values[i].GetSubrecords();
                for (auto &field_sr : srs)
                    weap->AddSubrecord(std::move(std::make_unique<Subrecord>(*field_sr)));
            }
        }
    }

    return records;
}
