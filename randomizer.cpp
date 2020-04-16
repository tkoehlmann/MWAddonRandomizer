#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cfloat>
#include "iohelpers.hpp"
#include "randomizer.hpp"
#include "randomizers/weapons.hpp"


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

    float weight_min = FLT_MAX;
    float weight_max = FLT_MIN;
    std::vector<float> weight_values;

    int32_t value_min = INT32_MAX;
    int32_t value_max = INT32_MIN;
    std::vector<int32_t> value_values;

    int16_t health_min = INT16_MAX;
    int16_t health_max = INT16_MIN;
    std::vector<int16_t> health_values;

    float speed_min = FLT_MAX;
    float speed_max = FLT_MIN;
    std::vector<float> speed_values;

    int16_t enchant_min = INT16_MAX;
    int16_t enchant_max = INT16_MIN;
    std::vector<int16_t> enchant_values;

    int8_t damage_min = INT8_MAX;
    int8_t damage_max = INT8_MIN;
    int8_t damage_chop_min = INT8_MAX;
    int8_t damage_chop_max = INT8_MIN;
    int8_t damage_slash_min = INT8_MAX;
    int8_t damage_slash_max = INT8_MIN;
    int8_t damage_thrust_min = INT8_MAX;
    int8_t damage_thrust_max = INT8_MIN;
    std::vector<int8_t> damage_values;
    std::vector<int8_t> chop_values;
    std::vector<int8_t> slash_values;
    std::vector<int8_t> thrust_values;

    std::vector<int32_t> resistance_values; // resistance min/max is always 0/1
    std::vector<Weapons::AdditionalData> model_values; // Those things will always stay together

    auto rng = [&settings](int i) { return settings.GetNext(i); };

    // Step 1: Fill min/max and value fields
    std::vector<int> bad_ids;
    for (int i = 0; i < records.size(); ++i)
    {
        Record &r = *records[i];
        if (Weapons::is_artifact(*records[i]) && !settings.WeaponShuffleAffectsArtifactWeapons)
            continue;

        std::vector<Subrecord> srs = records[i]->GetSubrecords("WPDT"); // r["WPDT"];
        uint8_t *wpdt = srs[0].GetData();

/*
        DEBUG!
        This stuff doesn't work :(

        uint8_t *wpdt2 = records[i]->GetSubrecords("WPDT")[0].GetData();
        uint8_t *wpdt3 = records[i]->GetSubrecords("WPDT").at(0).GetData();

        int cmp2 = std::memcmp(wpdt, wpdt2, 32);
        int cmp3 = std::memcmp(wpdt, wpdt3, 32);
*/

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
            bad_ids.push_back(i);
            continue;
        }

        Weapons::set_min_max_values(weight_min, weight_max, weight_values, weight);
        Weapons::set_min_max_values(value_min, value_max, value_values, value);
        Weapons::set_min_max_values(health_min, health_max, health_values, health);
        Weapons::set_min_max_values(speed_min, speed_max, speed_values, speed);
        Weapons::set_min_max_values(enchant_min, enchant_max, enchant_values, enchant);
        Weapons::set_min_max_values(damage_chop_min, damage_chop_max, damage_min, damage_max, chop_values, damage_values, chop_min);
        Weapons::set_min_max_values(damage_chop_min, damage_chop_max, damage_min, damage_max, chop_values, damage_values, chop_max);
        Weapons::set_min_max_values(damage_slash_min, damage_slash_max, damage_min, damage_max, slash_values, damage_values, slash_min);
        Weapons::set_min_max_values(damage_slash_min, damage_slash_max, damage_min, damage_max, slash_values, damage_values, slash_max);
        Weapons::set_min_max_values(damage_thrust_min, damage_thrust_max, damage_min, damage_max, thrust_values, damage_values, thrust_min);
        Weapons::set_min_max_values(damage_thrust_min, damage_thrust_max, damage_min, damage_max, thrust_values, damage_values, thrust_max);
        resistance_values.push_back(io::read_dword(wpdt + offset_resistance_flag));

        model_values.push_back(Weapons::AdditionalData(records[i]));
    }

    // Step 2: Shuffle if necessary
    if (settings.WeaponsWeight != ShuffleType::None)
        std::random_shuffle(weight_values.begin(), weight_values.end(), rng);
    if (settings.WeaponsValue != ShuffleType::None)
        std::random_shuffle(value_values.begin(), value_values.end(), rng);
    if (settings.WeaponsHealth != ShuffleType::None)
        std::random_shuffle(health_values.begin(), health_values.end(), rng);
    if (settings.WeaponsSpeed != ShuffleType::None)
        std::random_shuffle(speed_values.begin(), speed_values.end(), rng);
    if (settings.WeaponsEnchantPts != ShuffleType::None)
        std::random_shuffle(enchant_values.begin(), enchant_values.end(), rng);
    if (settings.WeaponsDamage != ShuffleType::None)
    {
        std::random_shuffle(damage_values.begin(), damage_values.end(), rng);
        std::random_shuffle(chop_values.begin(), chop_values.end(), rng);
        std::random_shuffle(slash_values.begin(), slash_values.end(), rng);
        std::random_shuffle(thrust_values.begin(), thrust_values.end(), rng);
    }
    if (settings.WeaponsResistance != ShuffleType::None)
        std::random_shuffle(resistance_values.begin(), resistance_values.end(), rng);
    if (settings.WeaponsModels != ShuffleType::None)
        std::random_shuffle(model_values.begin(), model_values.end(), rng);

    // Step 3: iteate over all records and put in shuffled values instead
    for (size_t i = 0; i < records.size(); i++)
    {
        if (std::find(bad_ids.begin(), bad_ids.end(), i) != bad_ids.end())
            continue; // skip bad indices

        std::vector<Subrecord> srs = records[i]->GetSubrecords("WPDT");
        uint8_t *wpdt = srs[0].GetData();
        std::pair<int8_t, int8_t> minmax;

        Weapons::random(settings, settings.WeaponsWeight, i, offset_weight, wpdt, weight_min, weight_max, weight_values, io::write_float);
        Weapons::random(settings, settings.WeaponsValue, i, offset_value, wpdt, value_min, value_max, value_values, io::write_dword);
        Weapons::random(settings, settings.WeaponsHealth, i, offset_health, wpdt, health_min, health_max, health_values, io::write_word);
        Weapons::random(settings, settings.WeaponsSpeed, i, offset_speed, wpdt, speed_min, speed_max, speed_values, io::write_float);
        Weapons::random(settings, settings.WeaponsEnchantPts, i, offset_enchant, wpdt, enchant_min, enchant_max, enchant_values, io::write_word);

        switch (settings.WeaponsDamage)
        {
        case ShuffleType::None:
            break;

        case ShuffleType::Random:
            minmax = std::minmax(
                settings.GetNext(damage_max - damage_min) + damage_min,
                settings.GetNext(damage_max - damage_min) + damage_min);
            wpdt[offset_chop_min] = minmax.first;
            wpdt[offset_chop_max] = minmax.second;
            minmax = std::minmax(
                settings.GetNext(damage_max - damage_min) + damage_min,
                settings.GetNext(damage_max - damage_min) + damage_min);
            wpdt[offset_slash_min] =  minmax.first;
            wpdt[offset_slash_max] =  minmax.second;
            minmax = std::minmax(
                settings.GetNext(damage_max - damage_min) + damage_min,
                settings.GetNext(damage_max - damage_min) + damage_min);
            wpdt[offset_thrust_min] = minmax.first;
            wpdt[offset_thrust_max] = minmax.second;
            break;

        case ShuffleType::Random_Chaos:
            minmax = std::minmax(
                settings.GetNext(damage_max * 2) + damage_min / 2,
                settings.GetNext(damage_max * 2) + damage_min / 2);
            wpdt[offset_chop_min] = minmax.first;
            wpdt[offset_chop_max] = minmax.second;
            minmax = std::minmax(
                settings.GetNext(damage_max * 2) + damage_min / 2,
                settings.GetNext(damage_max * 2) + damage_min / 2);
            wpdt[offset_slash_min] = minmax.first;
            wpdt[offset_slash_max] = minmax.second;
            minmax = std::minmax(
                settings.GetNext(damage_max * 2) + damage_min / 2,
                settings.GetNext(damage_max * 2) + damage_min / 2);
            wpdt[offset_thrust_min] = minmax.first;
            wpdt[offset_thrust_max] = minmax.second;
            break;

        case ShuffleType::Shuffled_Different:
            wpdt[offset_chop_min] =   chop_values  [i * 6 + 0];
            wpdt[offset_chop_max] =   chop_values  [i * 6 + 1];
            wpdt[offset_slash_min] =  slash_values [i * 6 + 2];
            wpdt[offset_slash_max] =  slash_values [i * 6 + 3];
            wpdt[offset_thrust_min] = thrust_values[i * 6 + 4];
            wpdt[offset_thrust_max] = thrust_values[i * 6 + 5];
            break;

        case ShuffleType::Shuffled_Same:
            wpdt[offset_chop_min] =   chop_values  [i * 2 + 0];
            wpdt[offset_chop_max] =   chop_values  [i * 2 + 1];
            wpdt[offset_slash_min] =  slash_values [i * 2 + 0];
            wpdt[offset_slash_max] =  slash_values [i * 2 + 1];
            wpdt[offset_thrust_min] = thrust_values[i * 2 + 0];
            wpdt[offset_thrust_max] = thrust_values[i * 2 + 1];
            break;

        default:
            break;
        }

        {
            int32_t rmin = 0, rmax = 1;
            Weapons::random(settings, settings.WeaponsResistance, i, offset_resistance_flag, wpdt, rmin, rmax, resistance_values, io::write_dword);
        }

        srs = records[i]->GetSubrecords("WPDT"); //r["WPDT"];
        size_t sz = srs[0].GetDataSize();
        srs[0].SetData(wpdt, sz);

        if (settings.WeaponsModels != ShuffleType::None)
        {
            records[i]->ClearNonIDSubrecords();
            for (auto field_sr : model_values[i].GetSubrecords())
                records[i]->AddSubrecord(field_sr);
        }
    }

    return records;
}
