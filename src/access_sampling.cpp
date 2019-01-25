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

    auto [event, dummy] = parse_metric(metric_name);

    if (pfm_wrapper_.metric_is_supported (event))
    {
        metric_properties.push_back (MetricProperty (metric_name, "", "Address").absolute_point ().value_uint ());
    }

    return metric_properties;
}

int32_t access_sampling::add_metric (const std::string &metric)
{
    auto [event, period] = parse_metric(metric);
    PerfEventAttribute perf_event_attr;
    pfm_wrapper_.get_perf_event(event, period, &perf_event_attr);

    BufferPtr buffer = std::make_shared<perf_buffer::TraceBuffer> ();
    perf_sampling_.event_open(&perf_event_attr, buffer);
    std::cout << "add_metric: " << std::this_thread::get_id() << " fd " << buffer->fd << '\n';

    buffer_mutex_.lock ();
    std::size_t id = thread_buffers_.size ();
    thread_buffers_.push_back (buffer);
    buffer_mutex_.unlock ();
    return id;
}

std::tuple<std::string, unsigned int> access_sampling::parse_metric (const std::string &metric)
{
    auto substrings = split(metric,'@');
    if(substrings.size() != 2)
    {
        throw std::invalid_argument("Invalid metric given");
    }
    return {substrings[0], std::stoul(substrings[1])};
}

SCOREP_METRIC_PLUGIN_CLASS (access_sampling, "access_sampling")