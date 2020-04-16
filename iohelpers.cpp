#include <cstring>
#include "iohelpers.hpp"

int8_t io::read_byte(FILE *f, size_t *bytes_read)
{
    uint8_t res;
    if (!fread(&res, 1, 1, f))
        throw "Error in io::read_byte";
    *bytes_read += 1;
    return res;
}

int16_t io::read_word(FILE *f, size_t *bytes_read)
{
    uint16_t res;
    uint8_t mem[2];
    if (fread(mem, 1, 2, f) < 2)
        throw "Error in io::read_word";
    *bytes_read += 2;
    res = (mem[1] <<  8) |
          (mem[0] <<  0) ;
    return res;
}

int32_t io::read_dword(FILE *f, size_t *bytes_read)
{
    uint32_t res;
    uint8_t mem[4];
    if (fread(mem, 1, 4, f) < 4)
        throw "Error in io::read_word";
    *bytes_read += 4;
    res = (mem[3] << 24) |
          (mem[2] << 16) |
          (mem[1] <<  8) |
          (mem[0] <<  0) ;
    return res;
}

std::string io::read_string(FILE *f, size_t *bytes_read)
{
    uint32_t len = io::read_dword(f, bytes_read) + 1; // + 1 for NULL terminator
    char mem[len];
    mem[len - 1] = '\0'; // Strings aren't NULL terminated in esm files (unless they are, thanks Bethesda!)
    if (fread(mem, sizeof(char), len - 1, f) < (len - 1))
        throw "Error in io::read_string";
    *bytes_read += len - 1;
    return std::string(mem);
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
    uint8_t mem[4];
    if (fread(mem, 1, 4, f) < 4)
        throw "Error in io::read_float";
    *bytes_read += 4;
    res = read_float(mem);
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

void io::write_bytes(FILE *f, uint8_t *data, size_t len)
{
    fwrite(data, 1, len, f);
}

float io::read_float(uint8_t *mem)
{
    float f = *(float *)mem;
    return f;
}

void io::write_float(uint8_t *mem, float f)
{
    mem[0] = ((uint8_t *)&f)[0];
    mem[1] = ((uint8_t *)&f)[1];
    mem[2] = ((uint8_t *)&f)[2];
    mem[3] = ((uint8_t *)&f)[3];
}

int32_t io::read_dword(uint8_t *mem)
{
    int32_t i = (mem[3] << 24) |
                (mem[2] << 16) |
                (mem[1] << 8) |
                (mem[0] << 0);
    return i;
}

void io::write_dword(uint8_t *mem, int32_t i)
{
    mem[0] = ((uint8_t *)&i)[0];
    mem[1] = ((uint8_t *)&i)[1];
    mem[2] = ((uint8_t *)&i)[2];
    mem[3] = ((uint8_t *)&i)[3];
}

int16_t io::read_word(uint8_t *mem)
{
    int32_t i = (mem[1] << 8) |
                (mem[0] << 0);
    return i;
}

void io::write_word(uint8_t *mem, int16_t i)
{
    mem[1] = ((uint8_t *)&i)[1];
    mem[0] = ((uint8_t *)&i)[0];
}

void io::write_bytes(uint8_t *buf, uint8_t *data, size_t len)
{
    memcpy(buf, data, len);
}
