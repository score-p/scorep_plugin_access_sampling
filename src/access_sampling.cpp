#include <scorep/plugin/plugin.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <perf_sampling.h>


using namespace scorep::plugin::policy;

std::mutex buffer_mutex;
std::vector<BufferPtr> thread_buffers;

class access_sampling : public scorep::plugin::base<access_sampling, async, per_thread, scorep_clock>
{
    public:
    using TimeValuePair = std::pair<scorep::chrono::ticks, double>;
    using MetricProperty = scorep::plugin::metric_property;

    access_sampling () : perf_sampling_ (), pfm_wrapper_ ()
    {
        std::cout << "Loading Metric Plugin\n";
    }

    std::vector<MetricProperty> get_metric_properties (const std::string &metric_name)
    {
        std::cout << "get metric properties called with: " << metric_name << '\n';

        std::vector<MetricProperty> metric_properties;

        if (pfm_wrapper_.metric_is_supported(metric_name))
        {
            metric_properties.push_back (
            MetricProperty (metric_name, "", "Address").absolute_point ().value_uint ());
        }

        return metric_properties;
    }

    int32_t add_metric (const std::string &event)
    {
        std::cout << "add metric called with: " << event << '\n';
        BufferPtr buffer = std::make_shared<TraceBuffer> ();

        std::lock_guard<std::mutex> lock (buffer_mutex);
        thread_buffers.push_back (buffer);
        int32_t id = thread_buffers.size () - 1;
        return id;
    }

    void start ()
    {
        std::cout << "Start\n";
    }

    void stop ()
    {
        std::cout << "Stop\n";
    }

    template <typename C> void get_all_values (int32_t id, C &cursor)
    {
        std::cout << "Trace Buffer of thread: " << thread_buffers[id]->tid << '\n';
    }

    private:
    PerfSampling perf_sampling_;
    PfmWrapper pfm_wrapper_;
};

SCOREP_METRIC_PLUGIN_CLASS (access_sampling, "access_sampling")