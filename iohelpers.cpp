#include "iohelpers.hpp"

uint8_t io::read_byte(FILE *f)
{
    uint8_t res;
    if (!fread(&res, 1, 1, f))
        throw "Error in io::read_byte";
    return res;
}

uint16_t io::read_word(FILE *f)
{
    uint16_t res;
    uint8_t data[2];
    if (fread(data, 1, 2, f) < 2)
        throw "Error in io::read_word";
    res = (data[1] <<  8) |
          (data[0] <<  0) ;
    return res;
}

uint32_t io::read_dword(FILE *f)
{
    uint32_t res;
    uint8_t data[4];
    if (fread(data, 1, 4, f) < 4)
        throw "Error in io::read_word";
    res = (data[3] << 24) |
          (data[2] << 16) |
          (data[1] <<  8) |
          (data[0] <<  0) ;
    return res;
}

std::string io::read_string(FILE *f)
{
    uint32_t len = io::read_dword(f) + 1; // + 1 for NULL terminator
    char data[len];
    if (fread(data, sizeof(char), len - 1, f) < (len - 1))
        throw "Error in io::read_string";
    data[len-1] = '\0'; // Strings aren't NULL terminated in esm files (unless they are, thanks Bethesda!)
    return std::string(data);
}

std::string io::read_record_id(FILE *f)
{
    char name[5];
    if (fread(name, 1, 4, f) < 4)
        throw "Error in io::read_record_id";
    name[4] = '\0';
    return std::string(name);
}

float io::read_float(FILE *f)
{
    float res;
    uint8_t data[4];
    if (fread(data, 1, 4, f) < 4)
        throw "Error in io::read_float";
    res = (data[3] << 24) |
          (data[2] << 16) |
          (data[1] <<  8) |
          (data[0] <<  0) ;
    return res;
}
