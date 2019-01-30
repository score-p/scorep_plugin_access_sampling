#pragma once

#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

extern "C"
{
#include <linux/perf_event.h>
#include <perfmon/pfmlib.h>
#include <perfmon/pfmlib_perf_event.h>
}

constexpr unsigned int EVENT_COUNT_MAX = 2;

using PerfEventAttribute = struct perf_event_attr;

class PfmWrapper
{
    public:
    PfmWrapper ();

    bool is_metric_supported (const std::string &metric_name);

    void get_perf_event (const std::string &metric_name, uint64_t sample_period, PerfEventAttribute *perf_attr);

    private:
    std::map<std::string, std::string> supported_events_ = {
        { "load", "MEM_UOPS_RETIRED.ALL_LOADS" },
        { "store", "MEM_UOPS_RETIRED.ALL_STORES" }
    };
    static bool is_initialized_;
};
