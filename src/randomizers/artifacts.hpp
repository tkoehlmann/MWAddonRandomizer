#ifndef __ARTIFACTS_HPP
#define __ARTIFACTS_HPP

#include "../settings.hpp"
#include "../records.hpp"

#include <vector>

namespace Artifacts
{
std::vector<Record *> Randomize(std::vector<Record *> records, Settings &settings);
}

#endif /* __ARTIFACTS_HPP */
