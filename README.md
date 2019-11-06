# AccessSamplingPlugin
This Score-P metric plugin samples load and store memory accesses. The plugin collects time series for load and store samples per thread of execution. Each sample contains the virtual address of the sampled access and the corresponding
timestamp. Under the hood, the plugin uses Linux Perf to record the PEBS (Processor Event-Based
Sampling) counter MEM_UOPS_RETIRED. The collection process works independently from Score-P,
which only initializes and finalizes the plugin. During the finalization phase, Score-P obtains the
measured time series of samples and stores them in the output OTF2 trace. Additionally, the plugin
creates a memory access trace file per thread that contains access samples with the following
details:

- Virtual address of memory access
- Timestamp of sampled access
- Type of access: load/store
- Memory level: from L1 to remote memory
- Instruction pointer: for determining source code location

This software was developed as part of the EC H2020 funded project NEXTGenIO (Project ID: 671951) http://www.nextgenio.eu.

## Dependencies
- Linux OS with Perf
- C++17 Compiler
- perfmon2
- Boost >= 1.61
- CMake >= 3.10
- Score-P >= 3.1

## Installation

Checkout the repository with all submodules
```
~> git clone --recursive https://github.com/score-p/access_sampling
```

Prepare build with
```
~> mkdir access_sampling/BUILD
~> cd access_sampling/BUILD
```

Configure and install plugin
```
~> cmake -DCMAKE_INSTALL_
```
You can set the paths to perfmon2 using CMake variables `PFM_LIBRARIES`, `PFM_INCLUDE_DIRS` and `PFM_PREFIX`.

## Usage
First add the `/path/to/install/lib` to the `LD_LIBRARY_PATH`. The plugin requires an application
instrumented with Score-P. To make Score-P use the metric plugin, the following environment
variable needs to bet set:
```
~> export SCOREP_METRIC_PLUGINS=access_sampling
```
Next, set the counters with their overflow limits:
```
~> export SCOREP_METRIC_ACCESS_SAMPLING=Load@1800,Store@2000
```
The trigger limit can be set after "@". This limit specifies how often a sample is taken. In this
example, every 1800th load event and every 2000th store event is recorded. This parameter controls
the granularity of the sampling. Please note that this kind of sampling follows a strictly statistical
approach.
The path to the detailed memory access trace can be set with the following environment variable:
```
~> export AS_TRACE_PATH=/path/to/output
```
After a successful run the plugin reports how many events were produced for a specific counter and
thread like this:
```
Record Load metric on thread 139705538991872
Number of occurred access events: 873
```
If the internal buffer is too small, you will get the following message:
```
Event buffer was too small and events are overwritten
Number of occurred access events: 3239
```
In this case you can specify the buffer size (number of events) per thread and re-run the application.
```
~> export AS_SAMPLES_PER_THREAD=5000000
```
The internal buffer is a ring buffer. If its capacity is exhausted the oldest event in the buffer will be
overridden but the resulting trace will remain valid.
