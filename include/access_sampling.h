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

extern std::vector<BufferPtr> thread_buffers;

using namespace scorep::plugin::policy;
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

    private : PerfSampling perf_sampling_;
    PfmWrapper pfm_wrapper_;
};

template <typename CursorType> void access_sampling::get_all_values (int32_t id, CursorType &cursor)
{
    std::cout << "Trace Buffer of thread: " << thread_buffers[id]->tid << '\n';
}