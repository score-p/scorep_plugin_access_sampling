#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <boost/filesystem.hpp>
#include <catch.hpp>
#include <ctime>
#include <ring_buffer.h>
#include <trace_file.h>
#include <utils.h>

#define private public
#include <access_sampling.h>
#undef private

namespace bf = boost::filesystem;

inline bf::path
trace_path (ThreadId tid)
{
    std::stringstream ss;
    ss << "trace." << convert_thread_id (tid) << ".bin";
    return bf::path (ss.str ());
}

static uint64_t
timer ()
{
    return clock ();
}
class MetricConsumer
{
    public:
    MetricConsumer ()
    {
        sampling_.set_clock_handler (timer);
        auto metrics = sampling_.get_metric_properties (metric_name_);
        REQUIRE (metrics.size () == 1);
    }

    void
    stop ()
    {
        REQUIRE_NOTHROW (sampling_.stop ());
    }

    void
    thread_worker ()
    {
        sampling_.add_metric (metric_name_);
        compute (count_);
    }

    private:
    inline void
    compute (uint64_t count)
    {
        std::vector<int> a (count), b (count), c (count);
        std::generate (a.begin (), a.end (), []() { return std::rand (); });
        std::generate (b.begin (), b.end (), []() { return std::rand (); });

        for (uint64_t i = 0; i < count; i++)
        {
            c[i] = a[i] + b[i];
        }
    }

    private:
    static access_sampling sampling_;
    const std::string metric_name_ = "Load@400";
    const uint64_t count_ = 1 << 20;
};

access_sampling MetricConsumer::sampling_;

TEST_CASE ("Integration")
{
    std::vector<bf::path> traces;
    unsigned int nthreads = std::thread::hardware_concurrency ();

    const uint64_t start_time = timer ();
    {
        MetricConsumer mc;
        std::vector<std::thread> workers (nthreads);

        auto gen_thread = [&mc]() { return std::thread (&MetricConsumer::thread_worker, mc); };

        std::generate (workers.begin (), workers.end (), gen_thread);

        for (auto& w : workers)
        {
            if (w.joinable ())
            {
                traces.push_back (trace_path (w.get_id ()));
                w.join ();
            }
        }

        mc.stop ();
    }
    const uint64_t end_time = timer ();

    for (auto& trace : traces)
    {
        REQUIRE (bf::exists (trace));
        TraceFile tf (trace, TraceFileMode::READ);

        auto access_sequence = tf.read ();
        REQUIRE (access_sequence.size () > 0);
        for (auto& access : access_sequence)
        {
            REQUIRE (access.address > 0);
            REQUIRE (access.access_type != AccessType::NA);
            REQUIRE (start_time < access.time);
            REQUIRE (access.time < end_time);
        }
        REQUIRE (bf::remove (trace));
    }

    std::cout << "Tested with " << nthreads << "threads.\n";
}