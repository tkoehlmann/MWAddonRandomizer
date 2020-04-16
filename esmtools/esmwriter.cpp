#include <vector>
#include "esmwriter.hpp"
#include "../settings.hpp"
#include "../records.hpp"
#include "../iohelpers.hpp"

bool WriteESMFile(Settings &settings, std::vector<Record *> records, std::vector<std::pair<std::string, size_t>> master_files_size)
{
    FILE *f = fopen(settings.GetPluginFullPath().c_str(), "wb");

    uint8_t *tes3_data;
    size_t tes3_size;

    // Write TES3 record
    Record tes3 = Record("TES3");
    {
        uint8_t HEDR[300] = {};
        io::write_float(HEDR + 0, 1.3f);
        io::write_dword(HEDR + 296, records.size());
        tes3.AddSubrecord(Subrecord("HEDR", HEDR, 300));
        for (auto file_size : master_files_size)
        {
            tes3.AddSubrecord(Subrecord("MAST", file_size.first));
            uint64_t DATA = file_size.second;
            tes3.AddSubrecord(Subrecord("DATA", (uint8_t*)&DATA, 8));
        }

        tes3_size = tes3.GetRecordSize();
        size_t remaining = tes3_size;
        tes3_data = (uint8_t *)malloc(tes3_size);
        tes3.WriteRecord(tes3_data, &remaining);
    }

    fwrite(tes3_data, 1, tes3_size, f);
    fclose(f);

    return true;
}
