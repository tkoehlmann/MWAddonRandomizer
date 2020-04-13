#ifndef __SETTINGS_HPP_
#define __SETTINGS_HPP_

#include <string>
#include <vector>

/**
 * To note: Shuffled options will lead to a somewhat more consistent gameplay experience
 * while the Random options might make the game significantly more.
 *
 * For some things (i.e. alchemical effects) there are no differences between the two shuffle types and the two random types.
 */
enum class ShuffleType
{
    None,               // There is no randomization going on for the setting
    Shuffled_Same,      // The values are shuffled but only inside the same type (i.e. weapon thrust <-> weapon thrust)
    Shuffled_Different, // The values are shuffled but also between types (i.e. weapon slash <-> weapon thrust)
    Random,             // The values are entirely random (values between minimum and maximum found)
    Random_Chaos,       // The values are entirely random (use with caution!)
};

class Settings {
public:
    Settings();
    uint32_t GetSeed(); // Returns the seed ID
    uint32_t GetNext(); // Retrieves the next random number
    uint32_t GetNext(uint32_t i); // Retrieves the next random number in the range 0 to i-1
    void UpdateAffectedRecords();
    bool IsRecordAffected(std::string id);

    ShuffleType Artifacts;
    bool ConsiderMasterPropylonIndexAsArtifact = false;
    bool ConsiderRegularPropylonIndicesAsArtifacts = false;

    ShuffleType WeaponsWeight;      // Weight of weapons
    ShuffleType WeaponsValue;       // Value of weapons
    ShuffleType WeaponsHealth;      // Durability of weapons
    ShuffleType WeaponsSpeed;       // Weight of weapons
    ShuffleType WeaponsEnchantPts;  // Enchantablility of weapons
    ShuffleType WeaponsDamage;      // Damage of weapons
    ShuffleType WeaponsResistance;  // Whether weapons can ignore common weapon resistances
    ShuffleType WeaponsModels;      // The visuals of weapons, model and inventory display name (not to be confused with locations!)
    bool WeaponShuffleAffectsArtifactWeapons = false;

    /* ShuffleType AlchemyEffects; */
    /* etc. */

private:
    uint32_t m_seed;
    std::vector<std::string> *m_affected_records;
};

#endif /* __SETTINGS_HPP_ */
