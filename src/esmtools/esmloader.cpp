#include "esmloader.hpp"

#include "../iohelpers.hpp"
#include "../records.hpp"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <map>
#include <memory>
#include <string>

// Forward declarations
Record *read_record(_IO_FILE *f, Settings &settings);
bool read_header(FILE *f, size_t *record_size);
bool read_subrecords(Record *r, FILE *f, size_t record_size);

std::unordered_map<std::string, std::vector<Record *>> *ReadESMFile(std::string filepath, size_t *f_size,
                                                                    Settings &settings, size_t *total_file_size_bytes)
{
    FILE *f = fopen(filepath.c_str(), "rb");
    *f_size = io::get_file_size(f);
    *total_file_size_bytes += *f_size;
    auto result = new std::unordered_map<std::string, std::vector<Record *>>();
    if (f == nullptr)
        return nullptr;

    Record *r;
    while (((size_t)ftell(f) < *f_size) && ((r = read_record(f, settings)) != nullptr))
    {
        (*result)[r->GetID()].push_back(r);
    }

    fclose(f);
    return result;
}

Record *read_record(_IO_FILE *f, Settings &settings)
{
    Record *res;
    size_t dont_care   = 0;
    size_t record_size = 0;
    try
    {
        std::string id = io::read_record_id(f, &dont_care);
        bool unknown   = RecordToSubrecordTypes.find(id) == RecordToSubrecordTypes.end();
        if (unknown)
        {
            printf("Unknown record ID \"%s\" at file position 0x%08x (%ld)\n", id.c_str(), (unsigned int)ftell(f),
                   ftell(f));
            assert(false);
        }
        else
        {
            res = new Record(id);
            if (!read_header(f, &record_size))
            {
                printf("Error reading header data for record ID \"%s\"\n", res->GetID().c_str());
                exit(1);
            }

            res->Ignored = !settings.IsRecordAffected(res->GetID()) || unknown;
            if (res->Ignored)
            {
                // Just skip over it
                fseek(f, record_size, SEEK_CUR);
            }
            else
            {
                // Create subrecords
                read_subrecords(res, f, record_size);
            }
        }
    }
    catch (...)
    {
        if (res != nullptr)
            delete res;
        return nullptr;
    }

    return res;
}

bool read_header(FILE *f, size_t *record_size)
{
    try
    {
        size_t dont_care = 0;
        *record_size     = io::read_dword(f, &dont_care);
        io::read_dword(f, &dont_care); // This is an unknown field of 4 bytes
        io::read_dword(f, &dont_care); // Flags, not important here
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool read_subrecords(Record *r, FILE *f, size_t record_size)
{
    size_t bytecount = 0;

    try
    {
        while (bytecount < record_size)
        {
            std::string srid = io::read_record_id(f, &bytecount);
            auto sr          = std::make_unique<Subrecord>(
                new Subrecord(srid, RecordToSubrecordTypes[r->GetID()][srid], f, &bytecount));
            r->AddSubrecord(std::move(sr));
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}
