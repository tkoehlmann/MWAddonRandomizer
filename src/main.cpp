#include "globals/attributes.hpp"
#include "globals/globals.hpp"
#include "globals/magiceffects.hpp"
#include "globals/skills.hpp"
#include "randomizer.hpp"
#include "settings.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>


int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    std::vector<std::string> files = {
        "Morrowind.esm",
        "Tribunal.esm",
        "Bloodmoon.esm",
        "/home/tim/.config/openmw/data/plugins/masterindex/master_index.esp",
    };


    Settings settings;
    settings.MasterDataFilesDir = "";
    settings.PluginOutputDir    = "/mnt/ramdisk";
    settings.WeaponsWeight      = ShuffleType::Random;
    settings.WeaponsValue       = ShuffleType::Random;
    settings.WeaponsHealth      = ShuffleType::Random;
    settings.WeaponsSpeed       = ShuffleType::Random;
    settings.WeaponsEnchantPts  = ShuffleType::Random;
    settings.WeaponsDamage      = ShuffleType::Shuffled_Same;
    settings.WeaponsResistance  = ShuffleType::Shuffled_Same;
    settings.WeaponsModels      = ShuffleType::Random;
    settings.AlchemyEffects     = ShuffleType::Random_Chaos;

    settings.Artifacts                                     = ShuffleType::Random;
    settings.Uniques                                       = ShuffleType::Random;
    settings.ConsiderUniquesEqualToArtifacts               = false;
    settings.ShuffleMAINQuestRequirementArtifactsOrUniques = false;
    settings.ShufflePropylonIndices                        = true;
    settings.ShuffleMasterIndex                            = false;

    settings.UpdateAffectedRecords();

    auto start                   = std::chrono::high_resolution_clock::now();
    size_t total_file_size_bytes = Randomizer::Game(files, settings);
    auto finish                  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    printf("Finished reading and writing files, total size read: %0.1f MiB, total time: %f seconds\n",
           (double)total_file_size_bytes / (1024L * 1024L), elapsed.count());

    return 0;
}
