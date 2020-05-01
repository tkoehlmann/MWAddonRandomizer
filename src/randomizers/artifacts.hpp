#ifndef __ARTIFACTS_HPP
#define __ARTIFACTS_HPP

#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

namespace Artifacts
{
// vector<vector> because there's multiple different record types involved
std::vector<Record *> Randomize(std::vector<std::vector<Record *>> record_groups, Settings &settings);
} // namespace Artifacts

#endif /* __ARTIFACTS_HPP */
