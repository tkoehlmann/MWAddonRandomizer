#ifndef __ESMLOADER_HPP_
#define __ESMLOADER_HPP_

#include "../records.hpp"
#include "../settings.hpp"

#include <vector>

std::unordered_map<std::string, RecordCollection> *ReadESMFile(std::string filepath, size_t *f_size, Settings &settings,
                                                               size_t *total_file_size_bytes);

#endif /* __ESMLOADER_HPP_ */
