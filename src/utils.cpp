#include <cstdlib>
#include <cassert>
#include <cstdlib>
#include <iostream>

#include <utils.h>

std::string to_lower (const std::string &input_string)
{
    std::string lower ("");
    for (auto c : input_string)
    {
        lower.push_back (std::tolower (c));
    }
    return lower;
}

std::vector<std::string> split (const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream (s);
    while (std::getline (tokenStream, token, delimiter))
    {
        tokens.push_back (token);
    }
    return tokens;
}

std::tuple<std::string, unsigned int> parse_metric (const std::string &metric)
{
    auto substrings = split (metric, '@');
    if (substrings.size () != 2)
    {
        throw std::invalid_argument ("Invalid metric given");
    }
    return { substrings[0], std::stoul (substrings[1]) };
}

std::size_t read_buffer_size ()
{
    std::size_t buffer_size = 5e6;
    const char *size_string = std::getenv ("AS_SAMPLES_PER_THREAD");
    if (size_string != nullptr)
    {
        std::stringstream sstream (size_string);
        sstream >> buffer_size;
    }
    return buffer_size;
}

inline boost::filesystem::path read_trace_path_from_env ()
{
    const char *path_string = std::getenv ("AS_TRACE_PATH");
    if (path_string != nullptr)
    {
        return boost::filesystem::path{ path_string };
    }
    return boost::filesystem::current_path ();
}

boost::filesystem::path create_trace_directory ()
{
    namespace bf = boost::filesystem;
    auto parent_path = read_trace_path_from_env ();

    if (bf::exists (parent_path))
    {
        if (!bf::is_directory (parent_path))
        {
            std::cout << "Given trace path points not to a directory\n";
            std::cout << "Store trace in " << bf::current_path () << '\n';
            parent_path = bf::current_path ();
        }
    }
    else
    {
        bf::create_directory (parent_path);
    }
    return parent_path;
}
{
    assert(nbytes > 0);
    return nbytes / (1024 * 1024);
}

size_t convert_thread_id (std::thread::id tid)
{
    std::stringstream ss;
    ss << tid;
    return std::stoull (ss.str ());
}