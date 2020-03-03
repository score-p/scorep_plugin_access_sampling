#include <cstring>
#include <stdexcept>
#include <type_traits>

#include <scorep/plugin/plugin.hpp>

#include <perf_sampling.h>
#include <utils.h>

thread_local RingBufferMap PerfSampling::ring_buffers_;
thread_local EventBufferPtr PerfSampling::event_data_;

void
PerfSampling::set_event_buffer (EventBufferPtr event_buffer)
{
    event_data_ = event_buffer;
}

PerfSampling::PerfSampling ()
{
    initialize_signal_handler ();
}

void
PerfSampling::process_events (PerfRingBuffer* ring_buffer)
{

    void* current_pointer = nullptr;
    while ((current_pointer = ring_buffer->read ()) != nullptr)
    {
        auto* event = static_cast<UnknownEvent*> (current_pointer);

        switch (event->header.type)
        {
        /* only process sampling events */
        case PERF_RECORD_SAMPLE:
        {
            auto* access_event = static_cast<SamplingEvent*> (current_pointer);

            if (access_event->addr != 0)
            {
                AccessEvent event(scorep::chrono::measurement_clock::now ().count (),
                                  access_event->addr,
                                  access_event->ip,
                                  accessTypeFromPerf (access_event->data_src.mem_op),
                                  memoryLevelFromPerf (access_event->data_src.mem_lvl));
                if(event_data_)
                {
                    event_data_->append(event);
                }
                continue;
            }
            break;
        }
        case PERF_RECORD_THROTTLE:
        {
            // TODO debug msg.
            break;
        }
        case PERF_RECORD_UNTHROTTLE:
        {
            // TODO debug msg.
            break;
        }
        default:
            std::cerr << "Unknown record type: " << event->header.type << '\n';
            break;
        }
    }
}

void
PerfSampling::signal_handler ([[maybe_unused]] int signal, siginfo_t* info, [[maybe_unused]] void* context)
{
    disable (info->si_fd);
    auto ring_buffer_iter = ring_buffers_.find (info->si_fd);
    if (ring_buffer_iter != ring_buffers_.end ())
    {
        process_events (&ring_buffer_iter->second);
    }
    enable (info->si_fd);
}

void
PerfSampling::initialize_signal_handler ()
{
    struct sigaction action;
    std::memset (&action, 0, sizeof (struct sigaction));
    action.sa_flags = SA_RESTART;
    action.sa_flags |= SA_SIGINFO;
    action.sa_sigaction = (SignalHandlerFuncPtr)signal_handler;
    if (sigaction (SIGPROF, &action, NULL) < 0)
    {
        throw std::runtime_error ("Error: Unable to initialize signal handler");
    }
}

void
PerfSampling::event_open (PerfEventAttribute* attr)
{
    int fd = syscall (__NR_perf_event_open, attr, 0, -1, -1, 0);
    if (fd < 1)
    {
        throw std::runtime_error ("Error: perf_event_open failed.");
    }

    ring_buffers_.emplace (std::make_pair (fd, fd));

    if (fcntl (fd, F_SETFL, O_ASYNC | O_NONBLOCK))
    {
        throw std::runtime_error ("Error: Failed to enable the generation of signals on the fd");
    }

    struct f_owner_ex fown_ex;
    fown_ex.type = F_OWNER_TID;
    fown_ex.pid = syscall (SYS_gettid);
    if (fcntl (fd, F_SETOWN_EX, (unsigned long)&fown_ex) < 0)
    {
        throw std::runtime_error ("Error: Setting the fd ownership failed.");
    }

    if (fcntl (fd, F_SETFD, FD_CLOEXEC))
    {
        throw std::runtime_error ("Error: Could not set close-on-exec flag on the fd.");
    }

    if (fcntl (fd, F_SETSIG, SIGPROF))
    {
        throw std::runtime_error ("Error: Unable to create a signal on the fd.");
    }
    if (ioctl (fd, PERF_EVENT_IOC_RESET, 0) < 0)
    {
        throw std::runtime_error ("Error: Could not reset perf event.");
    }
}

void
PerfSampling::enable ()
{
    for (auto& [fd, dummy] : ring_buffers_)
    {
        enable (fd);
    }
}

void
PerfSampling::disable ()
{
    sigset_t blocksig;
    sigemptyset (&blocksig);
    sigaddset (&blocksig, SIGPROF);
    sigprocmask (SIG_BLOCK, &blocksig, nullptr);
}

void
PerfSampling::enable (int fd)
{
    if (ioctl (fd, PERF_EVENT_IOC_ENABLE) < 0)
    {
        std::cerr << "Error: Could not enable perf.\n";
    }
}

void
PerfSampling::disable (int fd)
{
    if (ioctl (fd, PERF_EVENT_IOC_DISABLE) < 0)
    {
        std::cerr << "Error: Could not disable perf.\n";
    }
}