#ifndef __ARTIFACTS_HPP
#define __ARTIFACTS_HPP

#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

namespace Artifacts
{
// vector<vector> because there's multiple different record types involved
RecordCollection Randomize(std::vector<RecordCollection> record_groups, RecordCollection dialog_journal_recs,
                           RecordCollection info_recs, Settings &settings);
} // namespace Artifacts

#endif /* __ARTIFACTS_HPP */
