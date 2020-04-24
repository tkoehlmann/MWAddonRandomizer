#ifndef __ATTRIBUTES_HPP
#define __ATTRIBUTES_HPP

#include <string>
#include <vector>

namespace Attributes
{
extern const std::vector<std::string> Attributes;
bool GetID(std::string name, int8_t *id);
bool GetID(int8_t id, std::string &name);
} // namespace Attributes

#endif /* __ATTRIBUTES_HPP */
