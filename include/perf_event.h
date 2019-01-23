#pragma once

#include <cstring>
#include <iostream>
#include <map>
#include <string>

extern "C"
{
#include <linux/perf_event.h>
#include <perfmon/pfmlib.h>
#include <perfmon/pfmlib_perf_event.h>
}

using PerfEventAttribute = struct perf_event_attr;

class PerfEvent
{
    public:
    PerfEvent ()
    {
        pfm_initialize ();
    }
    bool metric_is_supported (const std::string &metric_name)
    {
        int ret = -1;
        try
        {
            std::string event = supported_events_.at (metric_name);
            ret = pfm_find_event(event.c_str());
        }
        catch (const std::out_of_range &e)
        {
            return false;
        }
        return ret != -1;
    }

    PerfEventAttribute get_perf_event (const std::string &metric_name);

    private:
    std::map<std::string, std::string> supported_events_ = {
        { "Load", "MEM_UOPS_RETIRED.ALL_LOADS" },
        { "Store", "MEM_UOPS_RETIRED.ALL_STORES" }
    };
};