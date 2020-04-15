#ifndef __RANDOMIZERS_WEAPONS_HPP_
#define __RANDOMIZERS_WEAPONS_HPP_

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
        std::unordered_map<std::string, Subrecord> GetSubrecords();
    private:
        std::unordered_map<std::string, Subrecord> m_subrecords;
        void init_sr(Record *r, std::string field);
    };

    template <typename T>
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

    bool is_artifact(Record &rec);

} // namespace Weapons

#endif /* __RANDOMIZERS_WEAPONS_HPP_ */
