#ifndef __SKILLS_HPP
#define __SKILLS_HPP

#include "../records.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace Skills
{
struct Skill
{
    int32_t id;
    std::unique_ptr<uint8_t[]> skdt;
    std::string description;
    Record *record;
};

std::vector<Skill> Get(std::vector<Record *> records);

} // namespace Skills

#endif /* __SKILLS_HPP */
