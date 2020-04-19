#ifndef __RANDOMIZERS_WEAPONS_HPP_
#define __RANDOMIZERS_WEAPONS_HPP_

#include <algorithm>
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
        std::vector<Subrecord> GetSubrecords();
    private:
        std::vector<Subrecord> m_subrecords;
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
                std::pair<int8_t, int8_t> minmax;

                switch (type)
                {
                case ShuffleType::None:
                    break;

                case ShuffleType::Random:
                    if (is_weapon && GlobalMin != nullptr && GlobalMax != nullptr)
                    {
                        minmax = std::minmax(
                            settings.GetNext(*GlobalMax - *GlobalMin) + *GlobalMin,
                            settings.GetNext(*GlobalMax - *GlobalMin) + *GlobalMin);
                        wpdt[offset_min] = minmax.first;
                        wpdt[offset_max] = minmax.second;
                    }
                    else
                        f(wpdt + offset_min, settings.GetNext((T)(Max - Min)) + Min);
                    break;

                case ShuffleType::Random_Chaos:
                    if (is_weapon && GlobalMin != nullptr && GlobalMax != nullptr)
                    {
                        minmax = std::minmax(
                            settings.GetNext(*GlobalMax * 2) + *GlobalMin / 2,
                            settings.GetNext(*GlobalMax * 2) + *GlobalMin / 2);
                        wpdt[offset_min] = minmax.first;
                        wpdt[offset_max] = minmax.second;
                    }
                    else
                        f(wpdt + offset_min, settings.GetNext((T)(Max * 2)));
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

    // template <typename T>
    // void random(Settings &settings, ShuffleType &type, int i, const size_t &offset, uint8_t *wpdt, T &min, T &max, std::vector<T> &values, void (*f)(uint8_t *, T))
    // {
    //     switch (type)
    //     {
    //     case ShuffleType::None:
    //         break;

    //     case ShuffleType::Random:
    //         io::write_float(wpdt + offset, settings.GetNext((T)(max - min)) + min);
    //         break;

    //     case ShuffleType::Random_Chaos:
    //         io::write_float(wpdt + offset, settings.GetNext((T)(max * 2)));
    //         break;

    //     case ShuffleType::Shuffled_Different:
    //     case ShuffleType::Shuffled_Same:
    //         io::write_float(wpdt + offset, values[i]);
    //         break;

    //     default:
    //         break;
    //     }
    // }

    bool is_artifact(Record &rec);

} // namespace Weapons

#endif /* __RANDOMIZERS_WEAPONS_HPP_ */
