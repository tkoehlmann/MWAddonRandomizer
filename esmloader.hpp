#ifndef __ESMLOADER_HPP_
#define __ESMLOADER_HPP_

#include "records.hpp"
#include "settings.hpp"

std::unordered_map<std::string, Record*>* ReadESMFile(std::string filepath, Settings& settings);

#endif /* __ESMLOADER_HPP_ */
