#include <access_sampling.h>
#include <utils.h>

access_sampling::access_sampling () : perf_sampling_ (), pfm_wrapper_ ()
{
    std::cout << "Loading Metric Plugin\n";
}

std::vector<MetricProperty> access_sampling::get_metric_properties (const std::string &metric_name)
{
    std::cout << "get metric properties called with: " << metric_name << '\n';

    std::vector<MetricProperty> metric_properties;

    auto [event, dummy] = parse_metric (metric_name);

    if (pfm_wrapper_.is_metric_supported (event))
    {
        metric_properties.push_back (MetricProperty (metric_name, "", "Address").absolute_point ().value_uint ());
    }

    return metric_properties;
}

int32_t access_sampling::add_metric (const std::string &metric)
{
    auto [event, period] = parse_metric (metric);
    PerfEventAttribute perf_event_attr;
    pfm_wrapper_.get_perf_event (event, period, &perf_event_attr);

    perf_sampling_.event_open (&perf_event_attr);

    buffer_mutex_.lock ();
    auto tid = std::this_thread::get_id ();
    auto buffer_iter = thread_event_buffers_.find (tid);
    if (buffer_iter == thread_event_buffers_.end ())
    {
        thread_event_buffers_[tid] = perf_sampling_.get_event_buffer ();
    }

    int32_t id = all_events_.size ();
    all_events_.push_back (std::make_tuple (tid, event));
    buffer_mutex_.unlock ();

    return id;
}

void access_sampling::start ()
{
    perf_sampling_.enable ();
}

void access_sampling::stop ()
{
    perf_sampling_.disable ();
}

SCOREP_METRIC_PLUGIN_CLASS (access_sampling, "access_sampling")