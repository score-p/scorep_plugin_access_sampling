#pragma once

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
using ThreadEventPair = std::tuple<ThreadId,std::string>;

class access_sampling : public scorep::plugin::base<access_sampling, async, per_thread, scorep_clock>
{
    public:
    access_sampling ();

    std::vector<MetricProperty> get_metric_properties (const std::string &metric_name);

    int32_t add_metric (const std::string &event);

    void start ();

    void stop ();

    template <typename CursorType> void get_all_values (int32_t id, CursorType &cursor);

    private:
    std::tuple<std::string, unsigned int> parse_metric (const std::string &metric);
    PerfSampling perf_sampling_;
    PfmWrapper pfm_wrapper_;
    std::mutex buffer_mutex_;
    std::map<ThreadId, EventBufferPtr> thread_event_buffers_;
    std::vector<ThreadEventPair> all_events_;
};

template <typename CursorType> void access_sampling::get_all_values (int32_t id, CursorType &cursor)
{
    auto & [tid, metric] = all_events_[id];
    std::cout << "Record " << metric << " metric on thread " << tid << '\n';
    if(tid != thread_event_buffers_[tid]->tid)
    {
        throw std::runtime_error("Something went wrong in signal handler");
    }

    auto event_type = perf_buffer::typeFromString(metric);

    for(auto & event: thread_event_buffers_.at(tid)->buffer)
    {
        if(event_type == event.access_type)
        {
            cursor.write(scorep::chrono::ticks(event.time), event.address);
        }
    }

    auto buffer_size = thread_event_buffers_.at(tid)->buffer.size();
    if(thread_event_buffers_.at(tid)->number_of_accesses > buffer_size)
    {
        std::cerr << "Event buffer was too small and events are overwritten\n";
    }

    std::cout << "Number of occured access events: " << thread_event_buffers_.at(tid)->number_of_accesses << '\n';

}