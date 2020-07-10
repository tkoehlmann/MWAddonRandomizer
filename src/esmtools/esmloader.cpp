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
Record *read_record(_IO_FILE *f, Settings &settings, Record **previous_DIAL);
bool read_header(FILE *f, size_t *record_size);
bool read_subrecords(Record *r, FILE *f, size_t record_size, Record *previous_DIAL);

std::unordered_map<std::string, std::vector<Record *>> *ReadESMFile(std::string filepath, size_t *f_size,
                                                                    Settings &settings, size_t *total_file_size_bytes)
{
    auto result = new std::unordered_map<std::string, std::vector<Record *>>();

    FILE *f = fopen(filepath.c_str(), "rb");
    if (f == nullptr)
        return result;

    *f_size = io::get_file_size(f);
    *total_file_size_bytes += *f_size;

    Record *r;
    Record *previous_DIAL;
    while (((size_t)ftell(f) < *f_size) && ((r = read_record(f, settings, &previous_DIAL)) != nullptr))
    {
        (*result)[r->GetID()].push_back(r);
    }

    fclose(f);
    return result;
}

Record *read_record(_IO_FILE *f, Settings &settings, Record **previous_DIAL)
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
                if (id == "DIAL")
                    *previous_DIAL = res;
                // Create subrecords
                read_subrecords(res, f, record_size, *previous_DIAL);
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

bool read_subrecords(Record *r, FILE *f, size_t record_size, Record *previous_DIAL)
{
    size_t bytecount = 0;

    try
    {
        while (bytecount < record_size)
        {
            std::string srid = io::read_record_id(f, &bytecount);
            r->AddSubrecord(Subrecord(srid, RecordToSubrecordTypes[r->GetID()][srid], f, &bytecount));
        }
    }
    catch (...)
    {
        return false;
    }

    if (r->GetID() == "INFO")
        r->Previous_DIAL = previous_DIAL;

    return true;
}
