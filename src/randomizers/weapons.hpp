#ifndef __RANDOMIZERS_WEAPONS_HPP_
#define __RANDOMIZERS_WEAPONS_HPP_

#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_map>
#include <vector>
#include "../records.hpp"
#include "../settings.hpp"
#include "../iohelpers.hpp"

namespace Weapons
{
    struct AdditionalData
    {
    public:
        AdditionalData(Record *r);
        std::vector<std::unique_ptr<Subrecord>> GetSubrecords();
    private:
        std::vector<std::unique_ptr<Subrecord>> m_subrecords;
        void init_sr(Record *r, std::string field);
    };

    template <typename T>
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
                Min = min_start;
                Max = max_start;
                GlobalMin = global_min;
                GlobalMax = global_max;
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

            void Randomize(const bool is_weapon, Settings &settings, const ShuffleType type, const int i, const size_t offset_min, const size_t offset_max, uint8_t *wpdt, void (*f)(uint8_t *, T), T ignore_numbers_higher_than = 20001)
            {
                bool use_global = is_weapon && GlobalMin != nullptr && GlobalMax != nullptr;
                std::pair<int8_t, int8_t> minmax;

                auto get_mean = [ignore_numbers_higher_than](std::vector<T> *vs) -> T {
                    T sum = 0;
                    size_t ignored = 0;
                    for (T n : *vs)
                    {
                        if (n > ignore_numbers_higher_than)
                            ignored++;
                        else
                            sum += n;
                    }
                    return sum / (vs->size() - ignored);
                };
                T mean = get_mean(use_global
                                      ? GlobalValues
                                      : &Values);

                auto get_std_deviation = [ignore_numbers_higher_than](T mean, std::vector<T> *vs) {
                    T standardDeviation = 0;
                    size_t ignored = 0;
                    for (T v : *vs)
                        if (v > ignore_numbers_higher_than)
                            ignored++;
                        else
                            standardDeviation += std::pow(v - mean, 2);
                    return std::sqrt(standardDeviation / (vs->size() - ignored));
                };
                T std_deviation = get_std_deviation(mean, use_global
                                                              ? GlobalValues
                                                              : &Values);

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
                        minmax = std::minmax(
                            (T)settings.GetNext(distribution, Min, Max),
                            (T)settings.GetNext(distribution, Min, Max));
                        wpdt[offset_min] = minmax.first;
                        wpdt[offset_max] = minmax.second;
                    }
                    else
                    {
                        float next = settings.GetNext(distribution, Min, Max);
                        f(wpdt + offset_min, next);
                    }
                    break;

                case ShuffleType::Shuffled_Different:
                    if (GlobalValues != nullptr)
                    {
                        minmax = std::minmax(
                            (*GlobalValues)[i * 2 + 0],
                            (*GlobalValues)[i * 2 + 1]);
                        f(wpdt + offset_min, minmax.first);
                        f(wpdt + offset_max, minmax.second);
                    }
                    else
                        f(wpdt + offset_min, Values[i]);
                    break;

                case ShuffleType::Shuffled_Same:
                    if (is_weapon)
                    {
                        minmax = std::minmax(
                            Values[i * 2 + 0],
                            Values[i * 2 + 1]);
                        f(wpdt + offset_min, minmax.first);
                        f(wpdt + offset_max, minmax.second);
                    }
                    else
                        f(wpdt + offset_min, Values[i]);
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

    bool is_artifact_or_unique(Record &rec);
    bool prevent_shuffle(Record &rec);

} // namespace Weapons

#endif /* __RANDOMIZERS_WEAPONS_HPP_ */
