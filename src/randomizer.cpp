#include "randomizer.hpp"

#include "esmtools/esmloader.hpp"
#include "esmtools/esmwriter.hpp"
#include "globals/globals.hpp"
#include "iohelpers.hpp"
#include "randomizers/alchemy.hpp"
#include "randomizers/artifacts.hpp"
#include "randomizers/weapons.hpp"

#include <algorithm>
#include <cfloat>
#include <cinttypes>
#include <cstdint>
#include <cstring>


int HasRecordWithName(std::unordered_map<std::string, std::unordered_map<std::string, int>> &entries, std::string file,
                      std::string name)
{
    if (entries.find(file) == entries.end())
        return -1;
    if (entries[file].find(name) == entries[file].end())
        return -1;
    return entries[file][name];
}


size_t Randomizer::Game(std::vector<std::string> &files, Settings &settings)
{
    size_t total_file_size_bytes = 0; // this is just for fun
    std::vector<std::pair<std::string, size_t>>
        master_files_sizes; // stores file sizes for master files as they are necessary to include into the resulting
                            // plugin file
    std::unordered_map<std::string, RecordCollection> file_records; // Records of each of the files to load

    for (std::string fname : files)
    {
        size_t fsize;

        // Read file records and save file size
        std::unordered_map<std::string, RecordCollection> *res =
            ReadESMFile(fname, &fsize, settings, &total_file_size_bytes);
        master_files_sizes.push_back(std::pair<std::string, size_t>(io::get_file_name(fname), fsize));

        if (res == nullptr)
            printf("Error reading file: %s", fname.c_str());
        else
        {
            for (auto element : *res)
            {
                std::string type      = element.first;
                RecordCollection recs = element.second;

                // Put records into their categories
                if (file_records.find(type) == file_records.end())
                    file_records[type] = recs;
                else
                {
                    // Merge records
                    for (std::pair<const std::string, std::shared_ptr<Record>> &r : recs)
                    {
                        const std::string name         = r.first;
                        std::shared_ptr<Record> record = r.second;

                        // There's a reason we ignore records (hint: they are not adequately implemented)
                        if (!record->Ignored)
                            file_records[type].Insert(record);
                    }
                }
            }

            // printf("In file: %s\n", fname.c_str());
            // for (auto element : *res)
            //     printf("  Record: %s, count: %ld\n", element.first.c_str(), element.second.size());
        }
    }

    std::vector<std::shared_ptr<Record>> records_to_write;

    // Care must be taken that two different randomize functions won't modify the same records!
    RecordCollection weapon_records          = Weapons::Randomize(file_records["WEAP"], settings);
    std::vector<Magic::Effect> magic_effects = Magic::ReadEffects(file_records["MGEF"]);
    std::vector<Skills::Skill> skills        = Skills::Get(file_records["SKIL"]);
    RecordCollection alchemy           = Alchemy::Randomize(file_records["INGR"], settings, magic_effects, skills);
    RecordCollection artifacts_uniques = Artifacts::Randomize(
        {
            file_records["WEAP"],
            file_records["ARMO"],
            file_records["CLOT"],
            file_records["LOCK"],
            file_records["MISC"],
        },
        file_records["DIAL"], file_records["INFO"], settings);

    // TODO: the same for other randomizers - maybe abstract this in the future, maybe not
    for (auto wrec : weapon_records)
        records_to_write.push_back(wrec.second);
    for (auto ingr : alchemy)
        records_to_write.push_back(ingr.second);
    for (auto artuniq : artifacts_uniques)
        records_to_write.push_back(artuniq.second);

    WriteESMFile(settings, records_to_write, master_files_sizes);

    return total_file_size_bytes;
}
