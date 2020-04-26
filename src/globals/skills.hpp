#ifndef __SKILLS_HPP
#define __SKILLS_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "../records.hpp"

namespace Skills
{

struct Skill
{
    int32_t id;
    std::unique_ptr<uint8_t[]> skdt;
    std::string description;
    Record *record;
};

std::vector<Skill> Get(std::vector<Record*> records);

}

#endif /* __SKILLS_HPP */
