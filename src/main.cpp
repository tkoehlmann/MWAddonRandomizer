#include "esmtools/esmloader.hpp"
#include "esmtools/esmwriter.hpp"
#include "randomizer.hpp"
#include "settings.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>


int main(int argc, char **argv)
{
    std::vector<std::pair<std::string, Randomizer::MaxWeaponValues>> files = {
        { "Morrowind.esm", Randomizer::MaxWeaponValues(20000, 20000, 20000, 20000) },
        { "Tribunal.esm", Randomizer::MaxWeaponValues(20000, 50000, 20000, 20000) },
        { "Bloodmoon.esm", Randomizer::MaxWeaponValues(20000, 20000, 20000, 20000) },
    };
    std::vector<std::pair<std::string, size_t>> master_files_sizes;
    std::unordered_map<std::string, std::vector<Record *>> file_records;

    Settings settings;
    settings.MasterDataFilesDir = "";
    settings.PluginOutputDir    = "/mnt/ramdisk";
    settings.WeaponsWeight      = ShuffleType::Random;
    settings.WeaponsValue       = ShuffleType::Shuffled_Same;
    settings.WeaponsHealth      = ShuffleType::Random;
    settings.WeaponsSpeed       = ShuffleType::Random;
    settings.WeaponsEnchantPts  = ShuffleType::Random;
    settings.WeaponsDamage      = ShuffleType::Shuffled_Same;
    settings.WeaponsResistance  = ShuffleType::Shuffled_Same;
    settings.WeaponsModels      = ShuffleType::Random;
    settings.UpdateAffectedRecords();


    auto start                   = std::chrono::high_resolution_clock::now();
    size_t total_file_size_bytes = 0;
    for (auto file : files)
    {
        std::string fname                         = file.first;
        Randomizer::MaxWeaponValues weapon_values = file.second;

        size_t fsize;
        std::unordered_map<std::string, std::vector<Record *>> *res =
            ReadESMFile(fname, &fsize, settings, &total_file_size_bytes);
        master_files_sizes.push_back(std::pair<std::string, size_t>(fname, fsize));

        if (res == nullptr)
            printf("Error reading file: %s", fname.c_str());
        else
        {
            for (auto element : *res)
            {
                std::string type            = element.first;
                std::vector<Record *> &recs = element.second;

                if (file_records.find(type) == file_records.end())
                    file_records[type] = std::vector<Record *>(recs);
                else
                {
                    // Merge records
                    for (Record *r : recs)
                    {
                        if (r->Ignored)
                            continue; // There's a reason we ignore records (hint: they are not adequately implemented)

                        std::string name = r->GetName();
                        int64_t pos      = -1;
                        if (name != "")
                            pos = HasRecordWithName(file_records[type], r->GetName());

                        // if the record doesn't have a name or can't be found then we can just add it
                        // otherwise replace the old one
                        if (name == "" || pos < 0)
                            file_records[type].push_back(r);
                        else
                            file_records[type][pos] = r;
                    }
                }
            }

            // printf("In file: %s\n", fname.c_str());
            // for (auto element : *res)
            //     printf("  Record: %s, count: %ld\n", element.first.c_str(), element.second.size());
        }
    }

    Randomizer::MaxWeaponValues weapon_values(20000, 20000, 20000, 20000);
    std::vector<Record *> records_to_write;
    // Care must be taken that two different randomize functions can't modify the same records!
    std::vector<Record *> weapon_records = Randomizer::RandomizeWeapons(file_records["WEAP"], settings, weapon_values);

    for (auto wrec :
         weapon_records) // TODO: the same for other randomizers - maybe abstract this in the future, maybe not
        records_to_write.push_back(wrec);

    WriteESMFile(settings, records_to_write, master_files_sizes);

    auto finish                           = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    printf("Finished reading and writing files, total size read: %0.1f MiB, total time: %f seconds\n",
           (double)total_file_size_bytes / (1024L * 1024L), elapsed.count());

    return 0;
}
