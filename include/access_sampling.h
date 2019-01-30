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

class access_sampling : public scorep::plugin::base<access_sampling, async, per_thread, scorep_clock>
{
    public:
    access_sampling ();

    std::vector<MetricProperty> get_metric_properties (const std::string &metric_name);

    int32_t add_metric (const std::string &event);

    void start ()
    {
    }

    void stop ()
    {
    }

    template <typename CursorType> void get_all_values (int32_t id, CursorType &cursor);

    private:
    std::tuple<std::string, unsigned int> parse_metric (const std::string &metric);
    PerfSampling perf_sampling_;
    PfmWrapper pfm_wrapper_;
    std::mutex buffer_mutex_;
    std::map<ThreadId, EventBufferPtr> thread_event_buffers_;
};

template <typename CursorType> void access_sampling::get_all_values (int32_t id, CursorType &cursor)
{
    for (auto [tid, event_buffer] : thread_event_buffers_)
    {
        std::cout << tid << " : " << event_buffer->tid << '\n';
    }
}