#include <access_sampling.h>

std::vector<std::string> split (const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream (s);
    while (std::getline (tokenStream, token, delimiter))
    {
        tokens.push_back (token);
    }
    return tokens;
}

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

std::tuple<std::string, unsigned int> access_sampling::parse_metric (const std::string &metric)
{
    auto substrings = split (metric, '@');
    if (substrings.size () != 2)
    {
        throw std::invalid_argument ("Invalid metric given");
    }
    return { substrings[0], std::stoul (substrings[1]) };
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