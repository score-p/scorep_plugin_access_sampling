#include <trace_file.h>

static auto ios_open_mode (TraceFileMode mode)
{
    switch (mode)
    {
    case TraceFileMode::READ:
        return std::ios::in;

    case TraceFileMode::WRITE:
        return std::ios::out;

    default:
        throw std::invalid_argument ("Unsupported open mode.");
    }
}

TraceFile::TraceFile (const std::string &file, TraceFileMode mode)
{
    auto ios_mode = ios_open_mode(mode);
    file_handle_.open(file, ios_mode | std::ios::binary);
}

TraceFile::~TraceFile ()
{
    file_handle_.close();
}

void TraceFile::write (const EventBuffer &event_buffer)
{
    // TODO implement
}

AccessSequence TraceFile::read ()
{
    // TODO implement
    return AccessSequence ();
}