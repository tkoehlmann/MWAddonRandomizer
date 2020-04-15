#include <cstdint>
#include <cstdio>
#include <string>
#include <map>
#include <cassert>

#include "esmloader.hpp"
#include "../iohelpers.hpp"
#include "../records.hpp"

// Forward declarations
Record* read_record(_IO_FILE *f, Settings& settings);
bool read_header(Record *r, FILE *f);
bool read_subrecords(Record *r, FILE *f);

std::unordered_map<std::string, std::vector<Record *>> *ReadESMFile(std::string filepath, size_t *f_size, Settings &settings, size_t *total_file_size_bytes)
{
    FILE *f = fopen(filepath.c_str(), "rb");
    *f_size = io::get_file_size(f);
    *total_file_size_bytes += *f_size;
    auto result = new std::unordered_map<std::string, std::vector<Record *>>();
    if (f == nullptr)
        return nullptr;

    Record *r;
    while   (
                (ftell(f) < *f_size) &&
                ((r = read_record(f, settings)) != nullptr)
            )
    {
        (*result)[r->GetID()].push_back(r);
    }

    fclose(f);
    return result;
}

Record* read_record(_IO_FILE *f, Settings& settings)
{
    Record *res;
    size_t dont_care = 0;
    try
    {
        std::string id = io::read_record_id(f, &dont_care);
        bool unknown = RecordToSubrecordTypes.find(id) == RecordToSubrecordTypes.end();
        if (unknown)
        {
            printf("Unknown record ID \"%s\" at file position 0x%08x (%ld)\n", id.c_str(), (unsigned int)ftell(f), ftell(f));
            assert(false);
        }
        else
        {
            res = new Record(id);
            if (!read_header(res, f))
            {
                printf("Error reading header data for record ID \"%s\"\n", res->GetID().c_str());
                exit(1);
            }

            res->Ignored = !settings.IsRecordAffected(res->GetID()) || unknown;
            if (res->Ignored)
            {
                // Just skip over it
                fseek(f, res->Size, SEEK_CUR);
            }
            else
            {
                // Create subrecords
                read_subrecords(res, f);
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

bool read_header(Record *r, FILE *f)
{
    try
    {
        size_t dont_care = 0;
        r->Size = io::read_dword(f, &dont_care);
        io::read_dword(f, &dont_care); // This is an unknown field of 4 bytes
        io::read_dword(f, &dont_care); // Flags, not important here
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool read_subrecords(Record *r, FILE *f)
{
    size_t bytecount = 0;

    try
    {
        while (bytecount < r->Size)
        {
            std::string srid = io::read_record_id(f, &bytecount);
            auto sr = Subrecord(srid, RecordToSubrecordTypes[r->GetID()][srid], f, &bytecount);
            r->AddSubrecord(sr);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}
