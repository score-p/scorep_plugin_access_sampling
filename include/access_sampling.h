#pragma once

#include <boost/filesystem/path.hpp>
#include <scorep/plugin/plugin.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <perf_sampling.h>

using namespace scorep::plugin::policy;
using ThreadId = std::thread::id;
using TimeValuePair = std::pair<scorep::chrono::ticks, double>;
using MetricProperty = scorep::plugin::metric_property;
using ThreadEventPair = std::tuple<ThreadId, std::string>;

void
write_event_buffer (const std::thread::id& tid,
                    const EventRingBuffer& event_buffer,
                    boost::filesystem::path trace_dir);

class access_sampling : public scorep::plugin::base<access_sampling, async, post_mortem, per_thread, scorep_clock>
{
    public:
    access_sampling ();

    std::vector<MetricProperty>
    get_metric_properties (const std::string& metric_name);

    int32_t
    add_metric (const std::string& metric);

    void
    start ();

    void
    stop ();

    template <typename CursorType>
    void
    get_all_values (int32_t id, CursorType& cursor);

    private:
    PerfSampling perf_sampling_;
    PfmWrapper pfm_wrapper_;
    std::mutex buffer_mutex_;
    std::map<ThreadId, EventBufferPtr> thread_event_buffers_;
    std::vector<ThreadEventPair> all_events_;
    boost::filesystem::path trace_dir_;

    std::size_t buffer_size_ = 0;
};

template <typename CursorType>
void
access_sampling::get_all_values (int32_t id, CursorType& cursor)
{
    auto & [tid, metric] = all_events_[id];
    std::cout << "Record " << metric << " metric on thread " << tid << '\n';

    if(!thread_event_buffers_.at(tid).get())
    {
        std::cerr << "Invalid thread buffer of TID " << tid << '\n';
        return;
    }

    auto event_type = accessTypeFromString(metric);
    auto event_buffer = thread_event_buffers_.at(tid);
    for(auto & event: *event_buffer)
    {
        if(event_type == event.access_type)
        {
            cursor.write(scorep::chrono::ticks(event.time), event.address);
        }
    }

    write_event_buffer(tid, *event_buffer, trace_dir_);

    auto buffer_size = event_buffer->size();
    if(event_buffer->access_count() > buffer_size)
    {
        std::cerr << "Event buffer was too small and events are overwritten\n";
    }

    std::cout << "Number of occured access events: " << thread_event_buffers_.at(tid)->access_count() << '\n';
}