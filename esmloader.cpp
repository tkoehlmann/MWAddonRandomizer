#include <cstdint>
#include <cstdio>
#include <string>
#include <map>
#include <cassert>

#include "esmloader.hpp"
#include "iohelpers.hpp"
#include "records.hpp"

// Forward declarations
Record* read_record(_IO_FILE *f, Settings& settings);
bool read_header(Record *r, FILE *f);
bool read_subrecords(Record *r, FILE *f);


std::unordered_map<std::string, Record*>* ReadESMFile(std::string filepath, Settings& settings)
{
    printf("Start\n");
    FILE *f = fopen(filepath.c_str(), "rb");
    auto result = new std::unordered_map<std::string, Record*>();
    if (f == nullptr)
        return nullptr;

    Record *r;
    while   (
                !feof(f) &&
                ((r = read_record(f, settings)) != nullptr)
            )
    {
        (*result)[r->GetID()] = r;
    }

    fclose(f);
    return result;
}

Record* read_record(_IO_FILE *f, Settings& settings)
{
    Record *res;
    try
    {
        std::string id = io::read_record_id(f);
        bool unknown = RecordToSubrecordTypes.find(id) == RecordToSubrecordTypes.end();
        if (unknown)
        {
            printf("Unknown record ID \"%s\" at file position 0x%08x (%ld)\n", id.c_str(), (unsigned int)ftell(f), ftell(f));
            assert(false);
        }
        else
        {
            res = new Record(id);
            read_header(res, f);
            res->Ignored = !settings.IsRecordAffected(res->GetID()) || unknown;

            if (res->Ignored)
            {
                // Just skip over it
                fseek(f, res->Size, SEEK_CUR);
            }
            else
            {
                // Create subrecords
                if (!read_header(res, f))
                    printf("Error reading header data for record ID \"%s\"\n", res->GetID().c_str());
                else
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
        r->Size = io::read_dword(f);
        io::read_dword(f); // This is an unknown field of 4 bytes
        io::read_dword(f); // Flags, not important here
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
            std::string srid = io::read_record_id(f);

            Subrecord *sr = new Subrecord(r->GetID(), srid, RecordToSubrecordTypes[r->GetID()][srid], f, &bytecount);

            break; // Debug
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}
