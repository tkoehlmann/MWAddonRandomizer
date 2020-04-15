#include <vector>
#include "esmwriter.hpp"
#include "../settings.hpp"
#include "../records.hpp"
#include "../iohelpers.hpp"

bool WriteESMFile(Settings &settings, std::vector<Record *> records, std::vector<std::pair<std::string, size_t>> master_files_size)
{
    FILE *f = fopen(settings.GetPluginFullPath().c_str(), "wb");

    // Write TES3 record
    Record tes3 = Record("TES3");
    {
        uint8_t HEDR[300];
        io::write_float(HEDR + 0, 1.3f);
        io::write_dword(HEDR + 296, records.size());
        tes3.AddSubrecord(Subrecord("HEDR", HEDR, 300));
        for (auto file_size : master_files_size)
        {
            tes3.AddSubrecord(Subrecord("MAST", file_size.first));
            uint64_t DATA = file_size.second;
            tes3.AddSubrecord(Subrecord("DATA", (uint8_t*)&DATA, 8));
        }

        size_t sz = tes3.GetRecordSize();
        uint8_t tes3_data[sz];
        tes3.WriteRecord(tes3_data, &sz); // Uh oh, the order of subrecords is important... HOW DO I DO THIS? FUCK.
    }
    return false;
}
