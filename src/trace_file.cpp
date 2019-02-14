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
    file_.open(file, ios_mode | std::ios::binary);
}

TraceFile::~TraceFile ()
{
    file_.close();
}

void TraceFile::write (const EventBuffer &event_buffer)
{
    TraceMetaData md(event_buffer);
    write_meta_data(md);
    // write_raw_data(event_buffer);
}

AccessSequence TraceFile::read ()
{
    // TODO implement
    return AccessSequence ();
}

void TraceFile::write_meta_data(const TraceMetaData & md)
{
    file_ << tag_;
    file_.write((char *) &md, sizeof(TraceMetaData));
}

void TraceFile::write_raw_data(const void * data)
{
    // TODO implement
}

void TraceFile::read_meta_data(TraceMetaData * md)
{

}


void * TraceFile::read_raw_data(const TraceMetaData & md)
{
    return nullptr;
}

