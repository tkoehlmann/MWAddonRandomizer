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


int HasRecordWithName(std::vector<Record *> &records,
                      std::unordered_map<std::string, std::unordered_map<std::string, int>> &entries, std::string file,
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
    size_t total_file_size_bytes = 0;
    std::vector<std::pair<std::string, size_t>> master_files_sizes;
    std::unordered_map<std::string, std::vector<Record *>> file_records;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> entry_names;

    for (std::string fname : files)
    {
        size_t fsize;

        std::unordered_map<std::string, std::vector<Record *>> *res =
            ReadESMFile(fname, &fsize, settings, &total_file_size_bytes);
        master_files_sizes.push_back(std::pair<std::string, size_t>(io::get_file_name(fname), fsize));

        for (std::pair<std::string, std::vector<Record *>> file : *res)
            for (size_t i = 0; i < file.second.size(); i++)
                entry_names[file.first][file.second[i]->Name] = i;

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
                    // for (Record *r : recs)
                    for (size_t asd = 0; asd < recs.size(); ++asd)
                    {
                        Record *r = recs[asd];
                        if (r->Ignored)
                            continue; // There's a reason we ignore records (hint: they are not adequately implemented)

                        int64_t pos = -1;
                        // if (asd > 3400)
                        //     printf("ok\n");
                        if (r->Name != "")
                            pos = HasRecordWithName(file_records[type], entry_names, fname, r->Name);

                        // if the record doesn't have a name or can't be found then we can just add it
                        // otherwise replace the old one
                        if (r->Name == "" || pos < 0)
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

    std::vector<Record *> records_to_write;

    // Care must be taken that two different randomize functions can't modify the same records!
    std::vector<Record *> weapon_records = Weapons::Randomize(file_records["WEAP"], settings);

    std::vector<Magic::Effect> magic_effects = Magic::ReadEffects(file_records["MGEF"]);
    std::vector<Skills::Skill> skills        = Skills::Get(file_records["SKIL"]);
    std::vector<Record *> alchemy           = Alchemy::Randomize(file_records["INGR"], settings, magic_effects, skills);
    std::vector<Record *> artifacts_uniques = Artifacts::Randomize(
        {
            file_records["WEAP"],
            file_records["ARMO"],
            file_records["CLOT"],
            file_records["LOCK"],
            file_records["MISC"],
        },
        settings);

    // TODO: the same for other randomizers - maybe abstract this in the future, maybe not
    for (auto wrec : weapon_records)
        records_to_write.push_back(wrec);
    for (auto ingr : alchemy)
        records_to_write.push_back(ingr);
    for (auto artuniq : artifacts_uniques)
        records_to_write.push_back(artuniq);

    WriteESMFile(settings, records_to_write, master_files_sizes);

    return total_file_size_bytes;
}
