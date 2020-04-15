#ifndef __ESMTOOLS_ESMWRITER_HPP_
#define __ESMTOOLS_ESMWRITER_HPP_

#include <vector>
#include "../settings.hpp"
#include "../records.hpp"

bool WriteESMFile(Settings &settings, std::vector<Record *> records, std::vector<std::pair<std::string, size_t>> master_files_size);

#endif /* __ESMTOOLS_ESMWRITER_HPP_ */
