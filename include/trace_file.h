#pragma once
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include <sstream>

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
    struct TraceMetaData
    {
        uint64_t access_count;
        uint64_t tid;

        TraceMetaData()
        :access_count(0),tid(0)
        {}

        TraceMetaData(const EventBuffer &event_buffer)
        :access_count(event_buffer.number_of_accesses)
        {
            std::stringstream ss;
            ss << event_buffer.tid;
            tid = std::stoull(ss.str());
        }
    };

    void write_meta_data (const TraceMetaData & md);
    void write_raw_data (const void * data);
    void read_meta_data(TraceMetaData * md);
    void * read_raw_data(const TraceMetaData & md);

    private:
    std::fstream file_;
    static constexpr const char *key_tag_ = "ATRACE";
};