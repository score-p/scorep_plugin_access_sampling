#include <perf_sampling.h>
#include <stdexcept>
#include <type_traits>
#include <cstring>

extern "C"
{
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/syscall.h>
}

static thread_local ThreadData thread_data_;

using SignalHandlerFuncPtr = void (*) (int, siginfo_t *, void *);

static void perf_signal_handler(int signal, siginfo_t * info, void * context)
{

}

PerfSampling::PerfSampling ()
{
    initialize_signal_handler();
}

void PerfSampling::initialize_signal_handler ()
{
    struct sigaction action;
    std::memset (&action, 0, sizeof (struct sigaction));
    action.sa_flags = SA_RESTART;
    action.sa_flags |= SA_SIGINFO;
    action.sa_sigaction = (SignalHandlerFuncPtr) perf_signal_handler;
    if (sigaction (SIGPROF, &action, NULL) < 0)
    {
        throw std::runtime_error ("Error: Unable to initialize signal handler");
    }
}

std::size_t PerfSampling::event_open (PerfEventAttribute *attr)
{
    BufferPtr buffer = std::make_shared<perf_buffer::TraceBuffer> ();
    thread_data_.add_buffer (buffer);

    buffer->fd = syscall (__NR_perf_event_open, attr, 0, -1, -1, 0);
    if (buffer->fd < 1)
    {
        throw std::runtime_error ("Error: perf_event_open failed.");
    }

    if (fcntl (buffer->fd, F_SETFL, O_ASYNC | O_NONBLOCK))
    {
        throw std::runtime_error ("Error: Failed to enable the generation of signals on the fd");
    }

    struct f_owner_ex fown_ex;
    fown_ex.type = F_OWNER_TID;
    fown_ex.pid = syscall (SYS_gettid);
    if (fcntl (buffer->fd, F_SETOWN_EX, (unsigned long)&fown_ex) < 0)
    {
        throw std::runtime_error ("Error: Setting the fd ownership failed.");
    }

    if (fcntl (buffer->fd, F_SETFD, FD_CLOEXEC))
    {
        throw std::runtime_error ("Error: Could not set close-on-exec flag on the fd.");
    }

    allocate_ring_buffer (buffer.get ());

    if (fcntl (buffer->fd, F_SETSIG, SIGPROF))
    {
        throw std::runtime_error ("Error: Unable to create a signal on the fd.");
    }

    buffer_mutex_.lock ();
    thread_buffers_.push_back (buffer);
    std::size_t id = thread_buffers_.size () - 1;
    buffer_mutex_.unlock ();

    return id;
}