#include <string>
#include <cstdint>
#include <cstdio>
#include <chrono>

#include "esmtools/esmloader.hpp"
#include "esmtools/esmwriter.hpp"
#include "settings.hpp"
#include "randomizer.hpp"


int main(int argc, char **argv)
{

    std::vector<std::pair<std::string, Randomizer::MaxWeaponValues>> files =
    {
        {"Morrowind.esm", Randomizer::MaxWeaponValues(20000, 20000, 20000, 20000)},
        {"Tribunal.esm", Randomizer::MaxWeaponValues(20000, 50000, 20000, 20000)},
        //{"Bloodmoon.esm", Randomizer::MaxWeaponValues(?????, ?????, ?????, ?????)},
    };
    std::vector<std::pair<std::string, size_t>> master_files_sizes;

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_file_size_bytes = 0;
    for (auto file : files)
    {
        std::string fname = file.first;
        Randomizer::MaxWeaponValues weapon_values = file.second;

        Settings settings;
        settings.MasterDataFilesDir = "";
        settings.PluginOutputDir = "/mnt/ramdisk";
        settings.WeaponsWeight = ShuffleType::Random;
        settings.WeaponsValue = ShuffleType::Shuffled_Same;
        settings.WeaponsHealth = ShuffleType::Random;
        settings.WeaponsSpeed = ShuffleType::Random;
        settings.WeaponsEnchantPts = ShuffleType::Random;
        settings.WeaponsDamage = ShuffleType::Shuffled_Same;
        settings.WeaponsResistance = ShuffleType::Shuffled_Same;
        settings.WeaponsModels = ShuffleType::Random;
        settings.UpdateAffectedRecords();

        size_t fsize;
        std::unordered_map<std::string, std::vector<Record *>> *res = ReadESMFile(fname, &fsize, settings, &total_file_size_bytes);
        master_files_sizes.push_back(std::pair<std::string, size_t>(fname, fsize));

        if (res == nullptr)
            printf("Error reading file: %s", fname.c_str());
        else
        {
            for (auto element : *res)
            {
                // TODO: Merge
            }

            // Not correct, just for testing! This should be done with the merged data!
            (*res)["WEAP"] = Randomizer::RandomizeWeapons((*res)["WEAP"], settings, weapon_values);

            printf("In file: %s\n", fname.c_str());
            for (auto element : *res)
                printf("  Record: %s, count: %ld\n", element.first.c_str(), element.second.size());

            std::vector<Record*> records_to_write;
            for (auto element : (*res)["WEAP"])
                records_to_write.push_back(element);

            WriteESMFile(settings, records_to_write, master_files_sizes);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    printf("Finished reading files, total size: %0.1f MiB, time: %f seconds\n", (double)total_file_size_bytes / (1024L*1024L), elapsed.count());

    return 0;
}
