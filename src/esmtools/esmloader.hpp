#ifndef __ESMLOADER_HPP_
#define __ESMLOADER_HPP_

#include <vector>
#include "../records.hpp"
#include "../settings.hpp"

std::unordered_map<std::string, std::vector<Record *>> *ReadESMFile(std::string filepath, size_t *f_size, Settings &settings, size_t *total_file_size_bytes);

#endif /* __ESMLOADER_HPP_ */
