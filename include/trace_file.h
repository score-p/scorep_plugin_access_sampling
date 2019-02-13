#pragma once
#include <fstream>
#include <string>
#include <tuple>
#include <vector>

#include <trace_buffer.h>

using AccessSequence = std::vector<AccessEvent>;

enum class TraceFileMode
{
    READ,
    WRITE,
};

class TraceFile
{

    public:
    explicit TraceFile (const std::string &file, TraceFileMode mode);
    ~TraceFile ();
    void write (const EventBuffer &event_buffer);
    AccessSequence read ();

    private:
    std::fstream file_handle_;
};