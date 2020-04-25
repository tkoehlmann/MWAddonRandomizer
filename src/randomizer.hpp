#include "records.hpp"
#include "settings.hpp"

#include <vector>

// TODO: Make this file into a library interface and move the Randomize* functions and other data into files in the
// randomizers subdirectory!

namespace Randomizer
{
// Maximum amount of item value to consider for calculating average values (since there are
// such expensive weapons around that the average price would be unreasonably high)
struct MaxWeaponValues
{
    int32_t melee;
    int32_t bows;
    int32_t thrown;
    int32_t ammo;

    MaxWeaponValues(int32_t m, int32_t b, int32_t t, int32_t a) : melee(m), bows(b), thrown(t), ammo(a) {}
};

std::vector<Record *> RandomizeWeapons(std::vector<Record *> records, Settings &settings,
                                       MaxWeaponValues &maxweaponvalues);
std::vector<Record *> RandomizeAlchemy(std::vector<Record *> records, Settings &settings,
                                       std::unique_ptr<std::unordered_map<int32_t, Record *>> &magic_effects);
} // namespace Randomizer
