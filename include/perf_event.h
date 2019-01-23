#pragma once

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
        pfm_err_t ret = PFM_ERR_INVAL;
        try
        {
            pfm_perf_encode_arg_t arg;
            std::string event = supported_events_.at (metric_name);
            ret = pfm_get_os_event_encoding (event.c_str (), PFM_PLM1, PFM_OS_PERF_EVENT, &arg);
        }
        catch (const std::out_of_range &e)
        {
            return false;
        }
        return ret == PFM_SUCCESS;
    }

    PerfEventAttribute get_perf_event (const std::string &metric_name);

    private:
    std::map<std::string, std::string> supported_events_ = {
        { "Load", "MEM_UOPS_RETIRED.ALL_LOADS" },
        { "Store", "MEM_UOPS_RETIRED.ALL_STORES" }
    };
};