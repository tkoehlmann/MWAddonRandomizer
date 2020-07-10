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
    size_t skdt_size;
    std::shared_ptr<std::vector<uint8_t>> skdt;
    std::string description;
    Record *record;

    Skill();
    Skill(const Skill &other);         // cc
    Skill &operator=(const Skill rhs); // ac
};

std::vector<Skill> Get(std::vector<Record *> records);

} // namespace Skills

#endif /* __SKILLS_HPP */
