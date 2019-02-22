#pragma once

#include <algorithm>
#include <boost/filesystem.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

std::string to_lower (const std::string &input_string);

std::vector<std::string> split (const std::string &s, char delimiter);

std::tuple<std::string, unsigned int> parse_metric (const std::string &metric);

std::size_t read_buffer_size ();

boost::filesystem::path create_trace_directory ();

std::size_t to_mb (std::size_t nbytes);

size_t convert_thread_id (std::thread::id tid);