#include "iohelpers.hpp"

uint8_t io::read_byte(FILE *f, size_t *bytes_read)
{
    uint8_t res;
    if (!fread(&res, 1, 1, f))
        throw "Error in io::read_byte";
    *bytes_read += 1;
    return res;
}

uint16_t io::read_word(FILE *f, size_t *bytes_read)
{
    uint16_t res;
    uint8_t data[2];
    if (fread(data, 1, 2, f) < 2)
        throw "Error in io::read_word";
    *bytes_read += 2;
    res = (data[1] <<  8) |
          (data[0] <<  0) ;
    return res;
}

uint32_t io::read_dword(FILE *f, size_t *bytes_read)
{
    uint32_t res;
    uint8_t data[4];
    if (fread(data, 1, 4, f) < 4)
        throw "Error in io::read_word";
    *bytes_read += 4;
    res = (data[3] << 24) |
          (data[2] << 16) |
          (data[1] <<  8) |
          (data[0] <<  0) ;
    return res;
}

std::string io::read_string(FILE *f, size_t *bytes_read)
{
    uint32_t len = io::read_dword(f, bytes_read) + 1; // + 1 for NULL terminator
    char data[len];
    data[len - 1] = '\0'; // Strings aren't NULL terminated in esm files (unless they are, thanks Bethesda!)
    if (fread(data, sizeof(char), len - 1, f) < (len - 1))
        throw "Error in io::read_string";
    *bytes_read += len - 1;
    return std::string(data);
}

std::string io::read_record_id(FILE *f, size_t *bytes_read)
{
    char name[5] = {'\0', '\0', '\0', '\0', '\0'};
    if (fread(name, 1, 4, f) < 4)
        throw "Error in io::read_record_id";
    *bytes_read += 4;
    return std::string(name);
}

float io::read_float(FILE *f, size_t *bytes_read)
{
    float res;
    uint8_t data[4];
    if (fread(data, 1, 4, f) < 4)
        throw "Error in io::read_float";
    *bytes_read += 4;
    res = (data[3] << 24) |
          (data[2] << 16) |
          (data[1] <<  8) |
          (data[0] <<  0) ;
    return res;
}

size_t io::get_file_size(FILE *f)
{
    size_t prev = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t res = ftell(f);
    fseek(f, prev, SEEK_SET);
    return res;
}
