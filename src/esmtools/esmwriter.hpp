#ifndef __ESMTOOLS_ESMWRITER_HPP_
#define __ESMTOOLS_ESMWRITER_HPP_

#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

bool WriteESMFile(Settings &settings, std::vector<std::shared_ptr<Record>> &records,
                  std::vector<std::pair<std::string, size_t>> master_files_size);

#endif /* __ESMTOOLS_ESMWRITER_HPP_ */
