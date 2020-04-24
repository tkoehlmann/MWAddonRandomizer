#ifndef __SETTINGS_HPP_
#define __SETTINGS_HPP_

#include <random>
#include <string>
#include <vector>

/**
 * To note: Shuffled options will lead to a somewhat more consistent gameplay experience
 * while the Random options might make the game significantly more.
 *
 * For some things (i.e. alchemical effects) there are no differences between the two shuffle types and the two random
 * types.
 */
enum class ShuffleType
{
    None,               // There is no randomization going on for the setting
    Shuffled_Same,      // The values are shuffled but only inside the same type (i.e. weapon thrust <-> weapon thrust)
    Shuffled_Different, // The values are shuffled but also between types (i.e. weapon slash <-> weapon thrust)
    Random,             // The values are entirely random (values between minimum and maximum found)
    Random_Chaos,       // The values are entirely random (use with caution!)
};

class Settings
{
    public:
    Settings();
    uint32_t GetSeed();      // Returns the seed ID
    uint32_t GetNext();      // Retrieves the next random number
    uint32_t GetNext(int i); // Retrieves the next random number in the range 0 to i-1
    float GetNext(double i); // Retrieves the next random number in the range 0 to i-1
    template<typename T>
    float GetNext(std::normal_distribution<float> ndist, T min,
                  T max) // Retrieves the next number depending on the given normal distribution
    {
        while (true)
        {
            float res = ndist(m_rng);
            if (res >= min && res <= max)
                return res;
        }
    }
    void UpdateAffectedRecords();
    bool IsRecordAffected(std::string id);

    std::string MasterDataFilesDir;
    std::string PluginOutputDir;
    std::string PluginFileName;
    std::string GetMasterFileFullPath(std::string file);
    std::string GetPluginFullPath();

    // Caution: Can only work by changing cells which is not implemented and won't be for some time!
    ShuffleType Artifacts = ShuffleType::None;      // Shuffle artifacts?
    ShuffleType Uniques   = ShuffleType::None;      // Shuffle unique items?
    bool ConsiderUniquesEqualToArtifacts;           // Put artifacts and uniques into the same shuffle pool?
    bool ShuffleQuestRequirementArtifactsOrUniques; // To defeat Dagoth Ur you need Keening and Sunder, with this
                                                    // enabled those may be shuffled somewhere else
    bool ShufflePropylonIndices; // If left off the Propylon Indices and the Master Index can be acquired from the usual
                                 // locations

    ShuffleType WeaponsWeight     = ShuffleType::None; // Weight of weapons
    ShuffleType WeaponsValue      = ShuffleType::None; // Value of weapons
    ShuffleType WeaponsHealth     = ShuffleType::None; // Durability of weapons
    ShuffleType WeaponsSpeed      = ShuffleType::None; // Weight of weapons
    ShuffleType WeaponsEnchantPts = ShuffleType::None; // Enchantablility of weapons
    ShuffleType WeaponsDamage     = ShuffleType::None; // Damage of weapons
    ShuffleType WeaponsResistance = ShuffleType::None; // Whether weapons can ignore common weapon resistances
    ShuffleType WeaponsModels = ShuffleType::None; // The visuals of weapons, model and inventory display name (not to
                                                   // be confused with locations!)

    /* ShuffleType AlchemyEffects; */
    /* etc. */
    ShuffleType AlchemyWeight  = ShuffleType::None; // Weight of ingredients
    ShuffleType AlchemyValue   = ShuffleType::None; // Value of ingredients
    ShuffleType AlchemyEffects = ShuffleType::None; // What ingredients do (Shuffle: Same amount of each effect type,
                                                    // Random: completely random effects)


    private:
    uint32_t m_seed;
    std::default_random_engine m_rng;
    std::vector<std::string> *m_affected_records;
};

#endif /* __SETTINGS_HPP_ */
