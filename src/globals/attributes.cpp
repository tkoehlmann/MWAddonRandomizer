#include "attributes.hpp"

#include <algorithm>


const std::vector<std::string> Attributes::Attributes = { "Strength", "Intelligence", "Willpower",   "Agility",
                                                          "Speed",    "Endurance",    "Personality", "Luck" };

bool Attributes::GetID(std::string name, int8_t *id)
{
    auto itr = std::find(Attributes.begin(), Attributes.end(), name);
    *id      = std::distance(Attributes.begin(), itr);
    return itr != Attributes.end();
}

bool Attributes::GetID(int8_t id, std::string &name)
{
    bool ok = id >= 0 && (size_t)id < Attributes.size();
    if (ok)
        name = Attributes[id];
    return ok;
}
