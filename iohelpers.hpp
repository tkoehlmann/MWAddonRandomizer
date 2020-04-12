#ifndef __IOHELPERS_H_
#define __IOHELPERS_H_

#include <cstdint>
#include <cstdio>
#include <string>

namespace io
{
    uint8_t read_byte(FILE *f);
    uint16_t read_word(FILE *f);
    uint32_t read_dword(FILE *f);
    std::string read_string(FILE *f);
    std::string read_record_id(FILE *f);
    float read_float(FILE *f);
    size_t get_file_size(FILE *f);
} // namespace io

#endif // __IOHELPERS_H_
