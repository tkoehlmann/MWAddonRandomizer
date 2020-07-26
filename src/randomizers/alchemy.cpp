#include "alchemy.hpp"

#include "../globals/attributes.hpp"
#include "../iohelpers.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdint>
#include <numeric>
#include <random>
#include <set>


float weight(std::vector<uint8_t> &irdt)
{
    return io::read_float(irdt);
}
void weight(std::vector<uint8_t> &irdt, float v)
{
    io::write_float(irdt, v);
}
int32_t value(std::vector<uint8_t> &irdt)
{
    return io::read_dword(irdt, 4);
}
void value(std::vector<uint8_t> &irdt, int32_t v)
{
    io::write_dword(irdt, v, 4);
}
Alchemy::EffectData effect(std::vector<uint8_t> &irdt, size_t id)
{
    int32_t e, s, a;
    size_t c;
    e = io::read_dword(irdt, 8 + id * 4);
    s = io::read_dword(irdt, 24 + id * 4);
    a = io::read_dword(irdt, 40 + id * 4);
    c = 0;
    for (int32_t n : { e, s, a })
        if (n >= 0)
            ++c;

    return Alchemy::EffectData(e, s, a, c);
}
void effect(std::vector<uint8_t> &irdt, size_t id, Alchemy::EffectData &data)
{
    io::write_dword(irdt, data.effect_id, 8 + id * 4);
    io::write_dword(irdt, data.skill_id, 24 + id * 4);
    io::write_dword(irdt, data.attrib_id, 40 + id * 4);
}

template<typename T>
double mean(std::vector<T> *vs)
{
    int64_t sum = 0;
    for (T n : *vs)
        sum += n * 1000;
    return (double)((sum / 1000.0d) / (double)vs->size());
};

template<typename T>
double std_deviation(int64_t mean, std::vector<T> *vs)
{
    double standardDeviation = 0;
    for (int64_t v : *vs)
        standardDeviation += std::pow(v - mean, 2);
    return std::sqrt(standardDeviation / (double)vs->size());
};

template<typename T>
std::normal_distribution<float> get_distribution(std::vector<T> values, ShuffleType type)
{
    double m     = mean<T>(&values);
    double sigma = std_deviation<T>(m, &values);
    return std::normal_distribution<float>(m, type != ShuffleType::Random_Chaos ? sigma : sigma * 2);
}

template<typename T>
void minmax(T &min, T &max)
{
    if (min > max)
        std::swap(min, max);
}


RecordCollection Alchemy::Randomize(RecordCollection records, Settings &settings,
                                    std::vector<Magic::Effect> &magic_effects, std::vector<Skills::Skill> &skills)
{
    std::set<int32_t> available_effect_ids_set;
    std::set<int32_t> available_skill_ids_set;
    std::vector<float> ingredient_weights;
    float weight_min = FLT_MAX, weight_max = FLT_MIN;
    std::vector<int32_t> ingredient_values;
    int32_t values_min = INT32_MAX, values_max = INT32_MIN;
    std::vector<EffectData> ingredient_effects;
    std::vector<size_t> ingredient_effect_count;

    // Step 1: Collect data
    for (std::pair<const std::string, std::shared_ptr<Record>> rs : records)
    {
        std::shared_ptr<Record> &r     = rs.second;
        Subrecord &irdt                = *r->GetSubrecords("IRDT")[0];
        std::vector<uint8_t> irdt_data = *irdt.Data;

        float ingr_weight                    = weight(irdt_data);
        int32_t ingr_value                   = value(irdt_data);
        std::vector<EffectData> ingr_effects = {
            effect(irdt_data, 0),
            effect(irdt_data, 1),
            effect(irdt_data, 2),
            effect(irdt_data, 3),
        };

        ingredient_weights.push_back(ingr_weight);
        weight_min = std::min(weight_min, ingr_weight);
        weight_max = std::max(weight_max, ingr_weight);
        ingredient_values.push_back(ingr_value);
        values_min = std::min(values_min, ingr_value);
        values_max = std::max(values_max, ingr_value);
        for (auto effect : ingr_effects)
        {
            if (effect.effect_id >= 0) // don't store empty fields
            {
                available_effect_ids_set.insert(effect.effect_id);
                if (effect.skill_id >= 0)
                    available_skill_ids_set.insert(effect.skill_id);
                ingredient_effects.push_back(effect);
            }
        }
        ingredient_effect_count.push_back(ingr_effects[0].effect_count); // index doesn't matter
    }
    std::vector<int32_t> available_effect_ids(available_effect_ids_set.begin(), available_effect_ids_set.end());
    std::vector<int32_t> available_skill_ids(available_skill_ids_set.begin(), available_skill_ids_set.end());

    // Step 2: Shuffle
    if (settings.AlchemyWeight != ShuffleType::None)
        std::random_shuffle(ingredient_weights.begin(), ingredient_weights.end());
    if (settings.AlchemyValue != ShuffleType::None)
        std::random_shuffle(ingredient_values.begin(), ingredient_values.end());
    if (settings.AlchemyEffects != ShuffleType::None)
        std::random_shuffle(ingredient_effects.begin(), ingredient_effects.end());

    // Step 3: Reassign
    RecordCollection result;
    std::vector<Alchemy::EffectData>::iterator effect_it =
        ingredient_effects.begin(); // iterator pointing to the next-to-use effect
    size_t effect_count_sum =
        std::accumulate(ingredient_effect_count.begin(), ingredient_effect_count.end(),
                        0); // Necessary so we don't allocate too many effects so that each ingredient can get at least
                            // one effect (shuffle different and chaos options)

    size_t i = 0;
    for (std::pair<const std::string, std::shared_ptr<Record>> rs : records)
    {
        std::shared_ptr<Record> &record = rs.second;
        std::shared_ptr<Subrecord> irdt = record->GetSubrecords("IRDT")[0];

        std::vector<uint8_t> irdt_data = *irdt->Data;

        std::normal_distribution<float> dist = get_distribution(ingredient_weights, settings.AlchemyWeight);
        switch (settings.AlchemyWeight)
        {
            case ShuffleType::Shuffled_Same:
            case ShuffleType::Shuffled_Different:
                weight(irdt_data, ingredient_weights[i]);
                break;

            case ShuffleType::Random:
            case ShuffleType::Random_Chaos:
                weight(irdt_data, settings.GetNext(dist, weight_min, weight_max));
                break;

            case ShuffleType::None:
                break;

            default:
                break;
        }

        dist = get_distribution(ingredient_values, settings.AlchemyValue);
        switch (settings.AlchemyValue)
        {
            case ShuffleType::Shuffled_Same:
            case ShuffleType::Shuffled_Different:
                value(irdt_data, ingredient_values[i]);
                break;

            case ShuffleType::Random:
            case ShuffleType::Random_Chaos:
                value(irdt_data, settings.GetNext(dist, values_min, values_max));
                break;

            case ShuffleType::None:
                break;

            default:
                break;
        }

        switch (settings.AlchemyEffects)
        {
            case ShuffleType::Shuffled_Same:
                for (size_t n = 0; n < ingredient_effect_count[i]; ++n)
                {
                    effect(irdt_data, n, *effect_it);
                    effect_it++;
                }
                for (size_t n = ingredient_effect_count[i]; n < 4; ++n)
                {
                    // Clear remaining fields of their data
                    EffectData d(-1, 0, 0, ingredient_effect_count[i]);
                    effect(irdt_data, n, d);
                }
                break;

            case ShuffleType::Shuffled_Different:
            {
                size_t effect_count = 0;
                size_t dist         = std::distance(effect_it, ingredient_effects.end());
                if (dist == effect_count_sum)
                    effect_count = 1; // if we only have as many effects left to distribute as we have elements left
                                      // then we only take one
                else
                    effect_count = settings.GetNext(4) + 1; // otherwise take a random number between 1 and 4

                for (size_t n = 0; n < effect_count; ++n)
                {
                    effect(irdt_data, n, *effect_it);
                    effect_it++;
                    --effect_count_sum;
                }
                for (size_t n = effect_count; n < 4; ++n)
                {
                    // Clear remaining fields of their data
                    EffectData d(-1, 0, 0, effect_count);
                    effect(irdt_data, n, d);
                }
            }
            break;

            case ShuffleType::Random:
            {
                size_t effect_count = settings.GetNext(4) + 1;
                for (size_t n = 0; n < effect_count; ++n)
                {
                    uint32_t eid = available_effect_ids[settings.GetNext((int)available_effect_ids.size())];
                    uint32_t aid = 0, sid = 0;
                    if (magic_effects[eid].affects_attribute)
                        aid = settings.GetNext((int)Attributes::Attributes.size());
                    if (magic_effects[eid].affects_skill)
                        sid = available_skill_ids[settings.GetNext((int)available_skill_ids.size())];

                    Alchemy::EffectData e(eid, sid, aid, effect_count);

                    effect(irdt_data, n, *effect_it);
                    effect_it++;
                    --effect_count_sum;
                }
                for (size_t n = effect_count; n < 4; ++n)
                {
                    // Clear remaining fields of their data
                    EffectData d(-1, 0, 0, effect_count);
                    effect(irdt_data, n, d);
                }
            }
            break;

            case ShuffleType::Random_Chaos:
            {
                size_t effect_count = settings.GetNext(4) + 1;
                for (size_t n = 0; n < effect_count; ++n)
                {
                    size_t pos   = settings.GetNext((int)magic_effects.size());
                    int32_t eid  = magic_effects[pos].id;
                    uint32_t aid = 0, sid = 0;
                    if (magic_effects[eid].affects_attribute)
                        aid = settings.GetNext((int)Attributes::Attributes.size());
                    if (magic_effects[eid].affects_skill)
                        sid = skills[settings.GetNext((int)skills.size())].id;

                    Alchemy::EffectData e(eid, sid, aid, effect_count);

                    effect(irdt_data, n, *effect_it);
                    effect_it++;
                    --effect_count_sum;
                }
                for (size_t n = effect_count; n < 4; ++n)
                {
                    // Clear remaining fields of their data
                    EffectData d(-1, 0, 0, effect_count);
                    effect(irdt_data, n, d);
                }
            }
            break;

            case ShuffleType::None:
                break;

            default:
                break;
        }

        irdt->Data = std::make_shared<std::vector<uint8_t>>(irdt_data);
        result.Insert(record);
        ++i;
    }

    return result;
}
