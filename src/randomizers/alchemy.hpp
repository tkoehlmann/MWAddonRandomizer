#ifndef __ALCHEMY_HPP
#define __ALCHEMY_HPP

#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

namespace Alchemy
{
std::vector<Record *> RandomizeAlchemy(std::vector<Record *> records, Settings &settings);
}

#endif /* _ALCHEMY_HPP */
