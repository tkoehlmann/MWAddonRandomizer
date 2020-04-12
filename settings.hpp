#ifndef __SETTINGS_HPP_
#define __SETTINGS_HPP_

#include <string>
#include <vector>


enum class ShuffleType
{
    None,               // There is no randomization going on for the setting
    Random,             // The values are entirely random (between minimum and maximum found)
    Random_Chaos,       // The values are entirely random (use with caution!)
    Shuffled_Same,      // The values are shuffled but only inside the same type (i.e. weapon thrust <-> weapon thrust)
    Shuffled_Different, // The values are shuffled but also between types (i.e. weapon slash <-> weapon thrust)
};

class Settings {
public:
    Settings();
    uint32_t GetSeed();
    uint32_t GetNext();
    void UpdateAffectedRecords();
    bool IsRecordAffected(std::string id);
    
    ShuffleType Artifacts;
    bool ConsiderMasterPropylonIndexAsArtifact;
    bool ConsiderRegularPropylonIndicesAsArtifacts;
    
    ShuffleType Weapons;
    bool WeaponShuffleAffectsArtifactWeapons;
    
    /* ShuffleType AlchemyEffects; */
    /* etc. */
    
private:
    uint32_t m_seed;
    std::vector<std::string> *m_affected_records;
};

#endif /* __SETTINGS_HPP_ */
