#include <iostream>
#include <string>
#include <cstdint>
#include <cstdio>

#include "esmloader.hpp"
//#include "iohelpers.hpp"
//#include "records.hpp"
#include "settings.hpp"


int main(int argc, char **argv)
{
    Settings settings;
    std::string files[] = {"Morrowind.esm"/*, "Tribunal.esm", "Bloodmoon.esm"*/};

    for (std::string file : files)
    {
        auto res = ReadESMFile(file, settings);
        if (res == nullptr)
            printf("Error reading file: %s", file.c_str());
        // TODO: Merge
        for (auto element : *res)
        {
            //printf("Record: %s, count: %ld\n", element.first.c_str(), element.second.size());
            printf("Record: %s\n", element.first.c_str());
        }
    }

    return 0;
}




/*auto f = fopen("test.bin", "rb");
    uint32_t dword = io::read_dword(f);
    uint16_t word = io::read_word(f);
    uint8_t byte = io::read_byte(f);
    std::string hw = io::read_string(f);
    float fl = io::read_float(f);

    printf("dword  = 0x%8x\n", dword);
    printf("word   = 0x%8x\n", word);
    printf("byte   = 0x%8x\n", byte);
    printf("string = %s\n", hw.c_str());
    printf("float  = 0x%8x\n", fl);*/
