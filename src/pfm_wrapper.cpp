#include <algorithm>
#include <pfm_wrapper.h>
#include <utils.h>

bool PfmWrapper::is_initialized_ = false;

PfmWrapper::PfmWrapper ()
{
    if (!is_initialized_)
    {
        pfm_initialize ();
        is_initialized_ = true;
    }
}

bool PfmWrapper::is_metric_supported (const std::string &metric_name)
{
    int ret = -1;
    try
    {
        std::string event = supported_events_.at (to_lower(metric_name));
        ret = pfm_find_event (event.c_str ());
    }
    catch (const std::out_of_range &e)
    {
        return false;
    }
    return ret != -1;
}

void PfmWrapper::get_perf_event (const std::string &metric_name, uint64_t sample_period, PerfEventAttribute *perf_attr)
{
    std::memset (perf_attr, 0, sizeof (PerfEventAttribute));
    perf_attr->disabled = 1;
    perf_attr->exclude_kernel = 1;
    perf_attr->exclude_hv = 1;
    perf_attr->type = PERF_TYPE_RAW;
    perf_attr->sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_ADDR | PERF_SAMPLE_DATA_SRC;
    perf_attr->precise_ip = 2;
    perf_attr->mmap = 1;
    perf_attr->wakeup_events = 1;
    perf_attr->size = sizeof (PerfEventAttribute);
    perf_attr->sample_period = sample_period;

    PerfEventAttribute tmp_attr;
    pfm_perf_encode_arg_t arg;
    std::memset (&arg, 0, sizeof (arg));
    arg.size = sizeof (arg);
    arg.attr = &tmp_attr;

    std::string event = supported_events_.at (to_lower(metric_name));
    pfm_err_t ret = pfm_get_os_event_encoding (event.c_str (), PFM_PLM1, PFM_OS_PERF_EVENT, &arg);
    if (ret != PFM_SUCCESS)
    {
        throw std::runtime_error ("pfm could not find event encoding");
    }
    perf_attr->config = arg.attr->config;
}