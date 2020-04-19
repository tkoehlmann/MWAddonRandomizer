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

            void Randomize(const bool is_weapon, Settings &settings, const ShuffleType type, const int i, const size_t offset_min, const size_t offset_max, uint8_t *wpdt, void (*f)(uint8_t *, T))
            {
                bool use_global = is_weapon && GlobalMin != nullptr && GlobalMax != nullptr;
                std::pair<int8_t, int8_t> minmax;

                auto get_mean = [](std::vector<T> *vs) {
                    return std::accumulate(vs->begin(), vs->end(), 0) / vs->size();
                };
                T mean = get_mean(use_global
                                      ? GlobalValues
                                      : &Values);

                auto get_std_deviation = [](T mean, std::vector<T> *vs) {
                    T standardDeviation = 0;
                    for (T v : *vs)
                        standardDeviation += std::pow(v - mean, 2);
                    return std::sqrt(standardDeviation / vs->size());
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
                            (T)settings.GetNext(distribution),
                            (T)settings.GetNext(distribution));
                        wpdt[offset_min] = minmax.first;
                        wpdt[offset_max] = minmax.second;
                    }
                    else
                    {
                        float next = settings.GetNext(distribution);
                        f(wpdt + offset_min, next);
                    }
                    break;

                case ShuffleType::Shuffled_Different:
                    if (GlobalValues != nullptr)
                    {
                        f(wpdt + offset_min, (*GlobalValues)[i * 2 + 0]);
                        f(wpdt + offset_max, (*GlobalValues)[i * 2 + 1]);
                    }
                    else
                        f(wpdt + offset_min, Values[i]);
                    break;

                case ShuffleType::Shuffled_Same:
                    if (is_weapon)
                    {
                        f(wpdt + offset_min, Values[i * 2 + 0]);
                        f(wpdt + offset_max, Values[i * 2 + 1]);
                    }
                    else
                        f(wpdt + offset_min, Values[i]);
                    break;

                default:
                    break;
                }
            }
    };

    bool is_artifact(Record &rec);

} // namespace Weapons

#endif /* __RANDOMIZERS_WEAPONS_HPP_ */
