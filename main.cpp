#include <string>
#include <cstdint>
#include <cstdio>
#include <chrono>

#include "esmtools/esmloader.hpp"
#include "esmtools/esmwriter.hpp"
//#include "iohelpers.hpp"
//#include "records.hpp"
#include "settings.hpp"
#include "randomizer.hpp"


int main(int argc, char **argv)
{
    Settings settings;
    settings.MasterDataFilesDir = "";
    settings.PluginOutputDir = "/mnt/ramdisk";
    settings.WeaponsWeight = ShuffleType::Random;
    settings.WeaponsValue = ShuffleType::Random;
    settings.WeaponsHealth = ShuffleType::Random;
    settings.WeaponsSpeed = ShuffleType::Random;
    settings.WeaponsEnchantPts = ShuffleType::Random;
    settings.WeaponsDamage = ShuffleType::Random;
    settings.WeaponsResistance = ShuffleType::Random;
    settings.WeaponsModels = ShuffleType::Random;
    settings.UpdateAffectedRecords();

    std::string files[] = {"Morrowind.esm" /*, "Tribunal.esm", "Bloodmoon.esm"*/};
    auto start = std::chrono::high_resolution_clock::now();
    size_t total_file_size_bytes = 0;
    for (std::string file : files)
    {
        std::vector<std::pair<std::string, size_t>> master_files_sizes;
        size_t fsize;
        std::unordered_map<std::string, std::vector<Record *>> *res = ReadESMFile(file, &fsize, settings, &total_file_size_bytes);
        master_files_sizes.push_back(std::pair<std::string, size_t>(file, fsize));

        if (res == nullptr)
            printf("Error reading file: %s", file.c_str());
        else
        {
            for (auto element : *res)
            {
                // TODO: Merge
            }

            // Not correct, just for testing! This should be done with the merged data!
            (*res)["WEAP"] = Randomizer::RandomizeWeapons((*res)["WEAP"], settings);

            for (auto element : *res)
                printf("Record: %s, count: %ld\n", element.first.c_str(), element.second.size());

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




/*auto f = fopen("test.bin", "rb");
    uint32_t dword = io::read_dword(f);
    uint16_t word = io::read_word(f);
    uint8_t byte = io::read_byte(f);
    std::string hw = io::read_string(f);
    float fl = io::read_float(f);

    printf("dword  = 0x%8x\n", dword);
    printf("word   = 0x%8x\n", word);
    printf("byte   = 0x%8x\n", byte);
    printf("string = %s\n", hw.c_str());
    printf("float  = 0x%8x\n", fl);*/
