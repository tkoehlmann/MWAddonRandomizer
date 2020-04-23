#include "alchemy.hpp"

std::vector<Record *> Alchemy::RandomizeAlchemy(std::vector<Record *> records, Settings &settings)
{
    // for now just dump the information, no idea what that stuff does yet
    for (Record *r : records)
    {
        printf("ID: %s:\n", r->GetName().c_str());
        std::vector<std::unique_ptr<Subrecord>> srs = r->GetSubrecords("IRDT");
        uint8_t *irdt                               = srs[0]->GetData();
        printf("    weight : %f\n", *(float*)(irdt + 0));
        printf("    value  : %d\n", *(int32_t*)(irdt + 4));
        printf("    effects: %3d, %3d, %3d, %3d\n", *(int32_t*)(irdt +  8), *(int32_t*)(irdt + 12), *(int32_t*)(irdt + 16),
               *(int32_t*)(irdt + 20));
        printf("    skills : %3d, %3d, %3d, %3d\n", *(int32_t*)(irdt + 24), *(int32_t*)(irdt + 28), *(int32_t*)(irdt + 32),
               *(int32_t*)(irdt + 36));
        printf("    attribs: %3d, %3d, %3d, %3d\n", *(int32_t*)(irdt + 40), *(int32_t*)(irdt + 44), *(int32_t*)(irdt + 48),
               *(int32_t*)(irdt + 52));
    }
    return std::vector<Record *>();
}
