#ifndef __IOHELPERS_H_
#define __IOHELPERS_H_

#include <cstdint>
#include <cstdio>
#include <string>
#include <memory>

namespace io
{
// File access
int8_t read_byte(FILE *f, size_t *bytes_read);
int16_t read_word(FILE *f, size_t *bytes_read);
int32_t read_dword(FILE *f, size_t *bytes_read);
std::string read_string(FILE *f, size_t *bytes_read);
std::string read_record_id(FILE *f, size_t *bytes_read);
float read_float(FILE *f, size_t *bytes_read);
size_t get_file_size(FILE *f);

void write_bytes(FILE *f, uint8_t *data, size_t len);

// Memory access
float read_float(uint8_t *mem);
float read_float(std::unique_ptr<uint8_t[]> &mem, size_t offset = 0);
void write_float(uint8_t *mem, float v);
void write_float(std::unique_ptr<uint8_t[]> &mem, float f, size_t offset = 0);
int32_t read_dword(uint8_t *mem);
int32_t read_dword(std::unique_ptr<uint8_t[]> &mem, size_t offset = 0);
void write_dword(uint8_t *mem, int32_t i);
void write_dword(std::unique_ptr<uint8_t[]> &mem, int32_t i, size_t offset);
int16_t read_word(uint8_t *mem);
int16_t read_word(std::unique_ptr<uint8_t[]> &mem, size_t offset = 0);
void write_word(uint8_t *mem, int16_t i);
void write_word(std::unique_ptr<uint8_t[]> &mem, int16_t i, size_t offset = 0);

void write_byte(uint8_t *mem, int8_t byte); // To satisfy the randomizer parameter
void write_byte(std::unique_ptr<uint8_t[]> &mem, int8_t byte, size_t offset = 0);
void write_bytes(uint8_t *buf, uint8_t *data, size_t len);

} // namespace io

#endif // __IOHELPERS_H_
