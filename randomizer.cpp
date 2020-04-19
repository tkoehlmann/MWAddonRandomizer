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
 * - Don't shuffle arrows and bolts together with other weapons as they can not be fired
 * - Try shuffling arrows and bolts among each other, test if bows can shoot bolts and crossbows can shoot arrows
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

    const uint16_t type_bow = 9;
    const uint16_t type_crossbow = 10;
    const uint16_t type_thrown = 11;
    const uint16_t type_arrow = 12;
    const uint16_t type_bolt = 13;


    Weapons::MinMaxData<float> weight_melee(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<float> weight_bows(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<float> weight_thrown(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<float> weight_ammo(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    Weapons::MinMaxData<int32_t> value_melee(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int32_t> value_bows(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<int32_t> value_thrown(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int32_t> value_ammo(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    Weapons::MinMaxData<int16_t> health_melee(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int16_t> health_bows(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<int16_t> health_thrown(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int16_t> health_ammo(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    Weapons::MinMaxData<float> speed_melee(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<float> speed_bows(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<float> speed_thrown(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<float> speed_ammo(FLT_MAX, FLT_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    Weapons::MinMaxData<int16_t> enchant_melee(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int16_t> enchant_bows(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<int16_t> enchant_thrown(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int16_t> enchant_ammo(INT16_MAX, INT16_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    int8_t damage_melee_global_min;
    int8_t damage_melee_global_max;
    std::vector<int8_t> damage_melee_global_values;
    Weapons::MinMaxData<int8_t> damage_melee_chop(INT8_MAX, INT8_MIN, &damage_melee_global_min, &damage_melee_global_max, &damage_melee_global_values);
    Weapons::MinMaxData<int8_t> damage_melee_slash(INT8_MAX, INT8_MIN, &damage_melee_global_min, &damage_melee_global_max, &damage_melee_global_values);
    Weapons::MinMaxData<int8_t> damage_melee_thrust(INT8_MAX, INT8_MIN, &damage_melee_global_min, &damage_melee_global_max, &damage_melee_global_values);
    Weapons::MinMaxData<int8_t> damage_bows(INT8_MAX, INT8_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<int8_t> damage_thrown(INT8_MAX, INT8_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int8_t> damage_ammo(INT8_MAX, INT8_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    Weapons::MinMaxData<int32_t> resistance_melee(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int32_t> resistance_bows(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr); // Bows and crossbows are the same for now
    Weapons::MinMaxData<int32_t> resistance_thrown(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr);
    Weapons::MinMaxData<int32_t> resistance_ammo(INT32_MAX, INT32_MIN, nullptr, nullptr, nullptr); // Arrows and bolts are the same for now

    std::vector<Weapons::AdditionalData> model_values_melee; // Those things will always stay together (at least for now)
    std::vector<Weapons::AdditionalData> model_values_bows; // Those things will always stay together (at least for now)
    std::vector<Weapons::AdditionalData> model_values_thrown; // Those things will always stay together (at least for now)
    std::vector<Weapons::AdditionalData> model_values_ammo; // Those things will always stay together (at least for now)

    auto rng = [&settings](int i) { return settings.GetNext(i); };

    // Step 1: Fill min/max and value fields
    std::vector<Record *> melee_weapons;
    std::vector<Record *> marksman_weapons; // Currently bows and crossbows are treated equally, let's see how this works out
    std::vector<Record *> marksman_throw;
    std::vector<Record *> marksman_ammo; // Currently arrows and bolts are treated equally, let's see how this works out

    for (int i = 0; i < records.size(); ++i)
    {
        if (Weapons::is_artifact(*records[i]) && !settings.WeaponShuffleAffectsArtifactWeapons)
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

        if (weight == 0)
        {
            // This will stop randomizing damage values onto and off VFX_DefaultBolt and other nonsense
            continue;
        }

        switch (type)
        {
        case type_bow:
        case type_crossbow:
            weight_bows.Set(weight);
            value_bows.Set(value/*, 1000*/);
            health_bows.Set(health);
            speed_bows.Set(speed);
            enchant_bows.Set(enchant);
            damage_bows.Set(chop_min);
            damage_bows.Set(chop_max);
            resistance_bows.Set(io::read_dword(wpdt + offset_resistance_flag));
            model_values_bows.push_back(Weapons::AdditionalData(records[i]));
            marksman_weapons.push_back(records[i]);
            break;

        case type_thrown:
            weight_thrown.Set(weight);
            value_thrown.Set(value/*, 100*/);
            health_thrown.Set(health);
            speed_thrown.Set(speed);
            enchant_thrown.Set(enchant);
            damage_thrown.Set(chop_min);
            damage_thrown.Set(chop_max);
            resistance_thrown.Set(io::read_dword(wpdt + offset_resistance_flag));
            model_values_thrown.push_back(Weapons::AdditionalData(records[i]));
            marksman_throw.push_back(records[i]);
            break;

        case type_arrow:
        case type_bolt:
            weight_ammo.Set(weight);
            value_ammo.Set(value/*, 10*/);
            health_ammo.Set(health);
            speed_ammo.Set(speed);
            enchant_ammo.Set(enchant);
            damage_ammo.Set(chop_min);
            damage_ammo.Set(chop_max);
            resistance_ammo.Set(io::read_dword(wpdt + offset_resistance_flag));
            model_values_ammo.push_back(Weapons::AdditionalData(records[i]));
            marksman_ammo.push_back(records[i]);
            break;

        default:
            weight_melee.Set(weight);
            value_melee.Set(value/*, 10000*/);
            health_melee.Set(health);
            speed_melee.Set(speed);
            enchant_melee.Set(enchant);
            damage_melee_chop.Set(chop_min);
            damage_melee_chop.Set(chop_max);
            damage_melee_slash.Set(chop_min);
            damage_melee_slash.Set(chop_max);
            damage_melee_thrust.Set(chop_min);
            damage_melee_thrust.Set(chop_max);
            resistance_melee.Set(io::read_dword(wpdt + offset_resistance_flag));
            model_values_melee.push_back(Weapons::AdditionalData(records[i]));
            melee_weapons.push_back(records[i]);
            break;
        }
    }

    // Step 2: Shuffle if necessary
    if (settings.WeaponsWeight != ShuffleType::None)
    {
        std::random_shuffle(weight_melee.Values.begin(), weight_melee.Values.end(), rng);
        std::random_shuffle(weight_bows.Values.begin(), weight_bows.Values.end(), rng);
        std::random_shuffle(weight_thrown.Values.begin(), weight_thrown.Values.end(), rng);
        std::random_shuffle(weight_ammo.Values.begin(), weight_ammo.Values.end(), rng);
    }
    if (settings.WeaponsValue != ShuffleType::None)
    {
        std::random_shuffle(value_melee.Values.begin(), value_melee.Values.end(), rng);
        std::random_shuffle(value_bows.Values.begin(), value_bows.Values.end(), rng);
        std::random_shuffle(value_thrown.Values.begin(), value_thrown.Values.end(), rng);
        std::random_shuffle(value_ammo.Values.begin(), value_ammo.Values.end(), rng);
    }
    if (settings.WeaponsHealth != ShuffleType::None)
    {
        std::random_shuffle(health_melee.Values.begin(), health_melee.Values.end(), rng);
        std::random_shuffle(health_bows.Values.begin(), health_bows.Values.end(), rng);
        std::random_shuffle(health_thrown.Values.begin(), health_thrown.Values.end(), rng);
        std::random_shuffle(health_ammo.Values.begin(), health_ammo.Values.end(), rng);
    }
    if (settings.WeaponsSpeed != ShuffleType::None)
    {
        std::random_shuffle(speed_melee.Values.begin(), speed_melee.Values.end(), rng);
        std::random_shuffle(speed_bows.Values.begin(), speed_bows.Values.end(), rng);
        std::random_shuffle(speed_thrown.Values.begin(), speed_thrown.Values.end(), rng);
        std::random_shuffle(speed_ammo.Values.begin(), speed_ammo.Values.end(), rng);
    }
    if (settings.WeaponsEnchantPts != ShuffleType::None)
    {
        std::random_shuffle(enchant_melee.Values.begin(), enchant_melee.Values.end(), rng);
        std::random_shuffle(enchant_bows.Values.begin(), enchant_bows.Values.end(), rng);
        std::random_shuffle(enchant_thrown.Values.begin(), enchant_thrown.Values.end(), rng);
        std::random_shuffle(enchant_ammo.Values.begin(), enchant_ammo.Values.end(), rng);
    }
    if (settings.WeaponsDamage != ShuffleType::None)
    {
        std::random_shuffle(damage_melee_chop.Values.begin(), damage_melee_chop.Values.end(), rng);
        std::random_shuffle(damage_melee_slash.Values.begin(), damage_melee_slash.Values.end(), rng);
        std::random_shuffle(damage_melee_thrust.Values.begin(), damage_melee_thrust.Values.end(), rng);
        std::random_shuffle(damage_bows.Values.begin(), damage_bows.Values.end(), rng);
        std::random_shuffle(damage_thrown.Values.begin(), damage_thrown.Values.end(), rng);
        std::random_shuffle(damage_ammo.Values.begin(), damage_ammo.Values.end(), rng);
    }
    if (settings.WeaponsResistance != ShuffleType::None)
    {
        std::random_shuffle(resistance_melee.Values.begin(), resistance_melee.Values.end(), rng);
        std::random_shuffle(resistance_bows.Values.begin(), resistance_bows.Values.end(), rng);
        std::random_shuffle(resistance_thrown.Values.begin(), resistance_thrown.Values.end(), rng);
        std::random_shuffle(resistance_ammo.Values.begin(), resistance_ammo.Values.end(), rng);
    }
    if (settings.WeaponsModels != ShuffleType::None)
    {
        std::random_shuffle(model_values_melee.begin(), model_values_melee.end(), rng);
        std::random_shuffle(model_values_bows.begin(), model_values_bows.end(), rng);
        std::random_shuffle(model_values_thrown.begin(), model_values_thrown.end(), rng);
        std::random_shuffle(model_values_ammo.begin(), model_values_ammo.end(), rng);
    }

    // Step 3: iteate over all records and put in shuffled values instead
    for (auto weapon_type : { &melee_weapons, &marksman_weapons, &marksman_throw, &marksman_ammo })
    {
        for (size_t i = 0; i < weapon_type->size(); i++)
        {
            // std::vector<Subrecord> srs = (*weapon_type)[i]->GetSubrecords("WPDT");
            // uint8_t *wpdt = srs[0].GetData();
            Record *weap = (*weapon_type)[i];
            std::vector<std::unique_ptr<Subrecord>> wpdt_srs = weap->GetSubrecords("WPDT");
            uint8_t *wpdt = wpdt_srs[0]->GetData();

            std::string wt;
            if (weapon_type == &melee_weapons)
                wt = "melee";
            else if (weapon_type == &marksman_weapons)
                wt = "bows";
            else if (weapon_type == &marksman_throw)
                wt = "throwing";
            else if (weapon_type == &marksman_ammo)
                wt = "ammo";
            else
                wt = "WHAT";

            printf("\n\n");
            printf("type = %s - i = %3ld\n", wt.c_str(), i);
            printf("&wpdt_srs[0] = 0x%" PRIxPTR "\n", (intptr_t)&wpdt_srs[0]);

            Weapons::MinMaxData<float> *weight;
            Weapons::MinMaxData<int32_t> *value;
            Weapons::MinMaxData<int16_t> *health;
            Weapons::MinMaxData<float> *speed;
            Weapons::MinMaxData<int16_t> *enchant;
            Weapons::MinMaxData<int32_t> *resistance;
            std::vector<Weapons::AdditionalData> *model;

            uint16_t type = io::read_word(wpdt + offset_type);
            switch (type)
            {
            case type_bow:
            case type_crossbow:
                weight = &weight_bows;
                value = &value_bows;
                health = &health_bows;
                speed = &speed_bows;
                enchant = &enchant_bows;
                model = &model_values_bows;
                resistance = &resistance_bows;
                break;

            case type_thrown:
                weight = &weight_thrown;
                value = &value_thrown;
                health = &health_thrown;
                speed = &speed_thrown;
                enchant = &enchant_thrown;
                model = &model_values_thrown;
                resistance = &resistance_thrown;

            case type_arrow:
            case type_bolt:
                weight = &weight_ammo;
                value = &value_ammo;
                health = &health_ammo;
                speed = &speed_ammo;
                enchant = &enchant_ammo;
                model = &model_values_ammo;
                resistance = &resistance_ammo;
                break;

            default:
                weight = &weight_melee;
                value = &value_melee;
                health = &health_melee;
                speed = &speed_melee;
                enchant = &enchant_melee;
                model = &model_values_melee;
                resistance = &resistance_melee;
                break;
            }

            weight->Randomize(false, settings, settings.WeaponsWeight, i, offset_weight, 0, wpdt, io::write_float);
            value->Randomize(false, settings, settings.WeaponsValue, i, offset_value, 0, wpdt, io::write_dword);
            health->Randomize(false, settings, settings.WeaponsHealth, i, offset_health, 0, wpdt, io::write_word);
            speed->Randomize(false, settings, settings.WeaponsSpeed, i, offset_speed, 0, wpdt, io::write_float);
            enchant->Randomize(false, settings, settings.WeaponsEnchantPts, i, offset_enchant, 0, wpdt, io::write_word);
            resistance->Randomize(false, settings, settings.WeaponsResistance, i, offset_resistance_flag, 0, wpdt, io::write_dword);

            damage_melee_chop.Randomize(true, settings, settings.WeaponsDamage, i, offset_chop_min, offset_chop_max, wpdt, io::write_byte);
            damage_melee_slash.Randomize(true, settings, settings.WeaponsDamage, i, offset_slash_min, offset_slash_max, wpdt, io::write_byte);
            damage_melee_thrust.Randomize(true, settings, settings.WeaponsDamage, i, offset_thrust_min, offset_thrust_max, wpdt, io::write_byte);

            weap->ClearSubrecords({"WPDT"});
            wpdt_srs[0]->SetData(wpdt, wpdt_srs[0]->GetDataSize());
            weap->AddSubrecord(std::move(wpdt_srs[0]));


            if (settings.WeaponsModels != ShuffleType::None)
            {
                weap->ClearSubrecords({"MODL", "FNAM", "ITEX", "ENAM", "SCRI"});
                std::vector<std::unique_ptr<Subrecord>> srs = (*model)[i].GetSubrecords();
                for (auto &field_sr : srs)
                    weap->AddSubrecord(std::move(std::make_unique<Subrecord>(*field_sr)));
            }
        }
    }

    return records;
}
