#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <tuple>

std::string to_lower(const std::string & input_string);

std::vector<std::string> split (const std::string &s, char delimiter);

std::tuple<std::string, unsigned int> parse_metric (const std::string &metric);
