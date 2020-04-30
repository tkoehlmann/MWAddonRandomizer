#ifndef __RANDOMIZERS_WEAPONS_HPP_
#define __RANDOMIZERS_WEAPONS_HPP_

#include "../iohelpers.hpp"
#include "../records.hpp"
#include "../settings.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_map>
#include <vector>

namespace Weapons
{
bool prevent_shuffle(Record &rec);
std::vector<Record *> Randomize(std::vector<Record *> records, Settings &settings);

struct AdditionalData
{
    public:
    AdditionalData(Record *r);
    std::vector<std::unique_ptr<Subrecord>> GetSubrecords();

    private:
    std::vector<std::unique_ptr<Subrecord>> m_subrecords;
    void init_sr(Record *r, std::string field);
};

template<typename T>
struct MinMaxData
{
    public:
    T Min;
    T Max;
    T *GlobalMin;
    T *GlobalMax;
    std::vector<T> Values;
    std::vector<T> *GlobalValues;

    MinMaxData(T min_start, T max_start, T *global_min, T *global_max, std::vector<T> *global_values)
    {
        Min          = min_start;
        Max          = max_start;
        GlobalMin    = global_min;
        GlobalMax    = global_max;
        GlobalValues = global_values;

        if (GlobalMin != nullptr)
            *GlobalMin = min_start;
        if (GlobalMax != nullptr)
            *GlobalMax = max_start;
    }

    void Set(T value)
    {
        Min = std::min(Min, value);
        Max = std::max(Max, value);
        if (GlobalMin != nullptr)
            *GlobalMin = std::min(*GlobalMin, value);
        if (GlobalMax != nullptr)
            *GlobalMax = std::max(*GlobalMax, value);
        Values.push_back(value);
        if (GlobalValues != nullptr)
            GlobalValues->push_back(value);
    }

    void Randomize(const bool is_weapon, Settings &settings, const ShuffleType type, const int i,
                   const size_t offset_min, const size_t offset_max, std::unique_ptr<uint8_t[]> &wpdt,
                   void (*f)(std::unique_ptr<uint8_t[]> &, T, size_t))
    {
        bool use_global = is_weapon && GlobalMin != nullptr && GlobalMax != nullptr;
        std::pair<int8_t, int8_t> minmax;

        auto get_mean = [](std::vector<T> *vs) -> T {
            int64_t sum = 0;
            for (T n : *vs)
                sum += n * 1000;
            return (double)((sum / 1000.0d) / (double)vs->size());
        };

        double mean = get_mean(use_global ? GlobalValues : &Values);

        auto get_std_deviation = [](int64_t mean, std::vector<T> *vs) {
            double standardDeviation = 0;
            for (int64_t v : *vs)
                standardDeviation += std::pow(v - mean, 2);
            return std::sqrt(standardDeviation / vs->size());
        };
        double std_deviation = get_std_deviation(mean, use_global ? GlobalValues : &Values);

        std::normal_distribution<float> distribution(mean, std_deviation);
        if (type == ShuffleType::Random_Chaos)
            distribution = std::normal_distribution<float>(mean, std_deviation * 2);

        switch (type)
        {
            case ShuffleType::None:
                break;

            case ShuffleType::Random:
            case ShuffleType::Random_Chaos:
                if (use_global)
                {
                    minmax           = std::minmax((T)settings.GetNext(distribution, Min, Max),
                                         (T)settings.GetNext(distribution, Min, Max));
                    wpdt[offset_min] = minmax.first;
                    wpdt[offset_max] = minmax.second;
                }
                else
                {
                    float next = settings.GetNext(distribution, Min, Max);
                    f(wpdt, next, offset_min);
                }
                break;

            case ShuffleType::Shuffled_Different:
                if (GlobalValues != nullptr)
                {
                    minmax = std::minmax((*GlobalValues)[i * 2 + 0], (*GlobalValues)[i * 2 + 1]);
                    f(wpdt, minmax.first, offset_min);
                    f(wpdt, minmax.second, offset_max);
                }
                else
                    f(wpdt, Values[i], offset_min);
                break;

            case ShuffleType::Shuffled_Same:
                if (is_weapon)
                {
                    minmax = std::minmax(Values[i * 2 + 0], Values[i * 2 + 1]);
                    f(wpdt, minmax.first, offset_min);
                    f(wpdt, minmax.second, offset_max);
                }
                else
                    f(wpdt, Values[i], offset_min);
                break;

            default:
                break;
        }
    }
};

struct WeaponData
{
    int8_t *damage_global_min;
    int8_t *damage_global_max;
    std::vector<int8_t> *damage_global_values;

    Weapons::MinMaxData<float> weight;
    Weapons::MinMaxData<int32_t> value;
    Weapons::MinMaxData<int16_t> health;
    Weapons::MinMaxData<float> speed;
    Weapons::MinMaxData<int16_t> enchant;
    Weapons::MinMaxData<int8_t> damage_chop;
    Weapons::MinMaxData<int8_t> damage_slash;
    Weapons::MinMaxData<int8_t> damage_thrust;
    Weapons::MinMaxData<int32_t> resistance;
    std::vector<Weapons::AdditionalData> model_values; // Those things will always stay together (at least for now)
    std::vector<Record *> records;
    Settings &settings;
    std::function<uint32_t(int)> rng;

    WeaponData(Settings &s, int8_t *global_min, int8_t *global_max, std::vector<int8_t> *global_values);
    void Shuffle(Settings &settings);
};

} // namespace Weapons

#endif /* __RANDOMIZERS_WEAPONS_HPP_ */
