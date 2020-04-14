#include <algorithm>
#include <cstdint>
#include <cfloat>
#include "iohelpers.hpp"
#include "randomizer.hpp"

namespace Weapons
{
    struct AdditionalData
    {
        std::string MODL;
        std::string FNAM;
        std::string ITEX;
        uint8_t *ENAM;
        size_t ENAM_size;
        std::string SCRI;

        AdditionalData(Record &rec)
        {
            // todo: make a deep copy of all available subrecords
            init_str(rec, "MODL", MODL);
            init_str(rec, "FNAM", FNAM);
            init_str(rec, "ITEX", ITEX);
            init_str(rec, "SCRI", SCRI);

            ENAM = rec.HasSubrecord("ENAM")
                       ? rec["ENAM"].GetData()
                       : nullptr;
            ENAM_size = rec.HasSubrecord("ENAM")
                            ? rec["ENAM"].GetSize()
                            : 0;
        }

        private:
        void init_str(Record &r, std::string field, std::string &s)
        {
            s = r.HasSubrecord(field)
                ? std::string((char *)r[field].GetData())
                : "";
        }
    };

    template<typename T>
    void set_min_max_values(T &min, T &max, std::vector<T> &values, T value)
    {
        min = std::min(min, value);
        max = std::max(max, value);
        values.push_back(value);
    }
    template <typename T>
    void set_min_max_values(T &min, T &max, T &global_min, T &global_max, std::vector<T> &values, std::vector<T> &global_values, T value)
    {
        min = std::min(min, value);
        max = std::max(max, value);
        global_min = std::min(global_min, value);
        global_max = std::max(global_max, value);
        values.push_back(value);
        global_values.push_back(value);
    }

    template <typename T>
    void random(Settings &settings, ShuffleType &type, int i, const size_t &offset, uint8_t *wpdt, T &min, T &max, std::vector<T> &values, void (*f)(uint8_t *, T))
    {
        switch (type)
        {
        case ShuffleType::None:
            break;

        case ShuffleType::Random:
            io::write_float(wpdt + offset, settings.GetNext((T)(max - min)) + min);
            break;

        case ShuffleType::Random_Chaos:
            io::write_float(wpdt + offset, settings.GetNext((T)(max * 2)));
            break;

        case ShuffleType::Shuffled_Different:
        case ShuffleType::Shuffled_Same:
            io::write_float(wpdt + offset, values[i]);
            break;

        default:
            break;
        }
    }

    bool is_artifact(Record &rec)
    {
        std::string id = std::string((char*)rec["NAME"].GetData());
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
}

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
    for (Record *r : records)
    {
        if (Weapons::is_artifact(*r) && !settings.WeaponShuffleAffectsArtifactWeapons)
            continue;

        uint8_t *wpdt = (*r)["WPDT"].GetData();
        Weapons::set_min_max_values(weight_min, weight_max, weight_values, io::read_float(wpdt + offset_weight));
        Weapons::set_min_max_values(value_min, value_max, value_values, io::read_dword(wpdt + offset_value));
        Weapons::set_min_max_values(health_min, health_max, health_values, io::read_word(wpdt + offset_health));
        Weapons::set_min_max_values(speed_min, speed_max, speed_values, io::read_float(wpdt + offset_speed));
        Weapons::set_min_max_values(enchant_min, enchant_max, enchant_values, io::read_word(wpdt + offset_enchant));
        Weapons::set_min_max_values(damage_chop_min, damage_chop_max, damage_min, damage_max, chop_values, damage_values, (int8_t)wpdt[offset_chop_min]);
        Weapons::set_min_max_values(damage_chop_min, damage_chop_max, damage_min, damage_max, chop_values, damage_values, (int8_t)wpdt[offset_chop_max]);
        Weapons::set_min_max_values(damage_slash_min, damage_slash_max, damage_min, damage_max, slash_values, damage_values, (int8_t)wpdt[offset_slash_min]);
        Weapons::set_min_max_values(damage_slash_min, damage_slash_max, damage_min, damage_max, slash_values, damage_values, (int8_t)wpdt[offset_slash_max]);
        Weapons::set_min_max_values(damage_thrust_min, damage_thrust_max, damage_min, damage_max, thrust_values, damage_values, (int8_t)wpdt[offset_thrust_min]);
        Weapons::set_min_max_values(damage_thrust_min, damage_thrust_max, damage_min, damage_max, thrust_values, damage_values, (int8_t)wpdt[offset_thrust_max]);
        resistance_values.push_back(io::read_dword(wpdt + offset_resistance_flag));

        model_values.push_back(Weapons::AdditionalData(*r));
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
        uint8_t *wpdt = (*records[i])["WPDT"].GetData();
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

        (*records[i])["WPDT"].SetData(wpdt, (*records[i])["WPDT"].GetSize());
        if (settings.WeaponsModels != ShuffleType::None)
        {
            // todo: clear all record's subrecords and put (deep-copy) the subrecords from model_values

            if (model_values[i].MODL != "")
                (*records[i])["MODL"].SetData((uint8_t *)model_values[i].MODL.c_str(), model_values[i].MODL.length() + 1);
            if (model_values[i].FNAM != "")
                (*records[i])["FNAM"].SetData((uint8_t *)model_values[i].FNAM.c_str(), model_values[i].FNAM.length() + 1);
            if (model_values[i].ITEX != "")
                (*records[i])["ITEX"].SetData((uint8_t *)model_values[i].ITEX.c_str(), model_values[i].ITEX.length() + 1);
            if (model_values[i].ENAM != 0)
                (*records[i])["ENAM"].SetData(model_values[i].ENAM, model_values[i].ENAM_size);
            if (model_values[i].SCRI != "")
                (*records[i])["SCRI"].SetData((uint8_t *)model_values[i].SCRI.c_str(), model_values[i].SCRI.length() + 1);
        }
    }

    return records;
}
