#include <algorithm>
#include <vector>
#include <filesystem>
#include "settings.hpp"



Settings::Settings()
{
    m_seed = time(nullptr);
    m_affected_records = new std::vector<std::string>();
    MasterDataFilesDir = "/";
    PluginOutputDir = "~";
    PluginFileName = "MWAddonRandomizer_" + std::to_string(GetSeed()) + ".omwaddon";
    srand(m_seed);
}

uint32_t Settings::GetNext()
{
    return rand();
}

uint32_t Settings::GetNext(int i)
{

    return i == 0 ? 0 : rand() % i;
}

float Settings::GetNext(double i)
{
    const double epsilon = 1e-5;
    if (std::abs(i) <= epsilon)
        return 0.0f;

    const double multiplier = 1000.0; // good enough
    int v = i * multiplier;
    v = rand() % v;
    return (double)v / multiplier;
}

uint32_t Settings::GetSeed()
{
    return m_seed;
}

void Settings::UpdateAffectedRecords()
{
    #define ISRANDOM(x) ((x) != ShuffleType::None)
    m_affected_records->clear();

    bool weapons_affected =
        ISRANDOM(WeaponsWeight) ||
        ISRANDOM(WeaponsValue) ||
        ISRANDOM(WeaponsHealth) ||
        ISRANDOM(WeaponsSpeed) ||
        ISRANDOM(WeaponsEnchantPts) ||
        ISRANDOM(WeaponsDamage) ||
        ISRANDOM(WeaponsResistance) ||
        ISRANDOM(WeaponsModels);

    if (weapons_affected)
        m_affected_records->push_back("WEAP");

    if (Artifacts != ShuffleType::None)
    {
        if (weapons_affected)
            m_affected_records->push_back("WEAP");

        m_affected_records->push_back("ARMO");
        m_affected_records->push_back("CLOT");
        m_affected_records->push_back("MISC"); // Azura's Star, Bittercup, Propylon Indices
        m_affected_records->push_back("LOCK"); // Skeleton Key
    }
}

bool Settings::IsRecordAffected(std::string id)
{
    return std::find(m_affected_records->begin(), m_affected_records->end(), id) != m_affected_records->end();
}

std::string Settings::GetMasterFileFullPath(std::string file)
{
    return std::filesystem::path(MasterDataFilesDir) / file;
}

std::string Settings::GetPluginFullPath()
{
    return std::filesystem::path(PluginOutputDir) / PluginFileName;
}
