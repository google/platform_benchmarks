# platform_benchmarks

https://github.com/google/platform_benchmarks

[TOC]

## What are Platform Benchmark?
Platform Benchmarks are a benchmark suite to measure basic architectural
metrics of a platform. Currently it only consists of CPUtest and Analog
bencmarks.

CPUtest is a set of microbenchmarks to measure fundamental micro-architectural
characteristics of a CPU.

Analog benchmarks are synthesized benchmarks that mimics some relevant workload
in Google cloud. Since we have a very dyanmic workload, over the time these
Analog benchmarks would change or even become obsolete.

Some fundamental properties of CPUtest

*   Runs on linux command-line
*   Works on all ISAs of interest --- X86, POWER
*   Metric is a simple function of the test result and CPU frequency (in most cases).
*   Does NOT primarily rely on performance counters
*   Examples of characteristics
    * Back to back dependent ADD latency
    * Cache Load to use latency
    * Cache bandwidth
    * Branch prediction latency and sensitivity to history
    * Store to load forwarding

## Why CPUtest
*   New platform introduction involves a lot of benchmarking
    * Understanding WHY benchmark scores are unexpected or different across CPUs

*   Finding performance bugs and features that may be off in Silicon.
    * Often vendors cannot give us a clear picture on early Silicon
    * Tracking progress on bug fixes independently.

*   Finding errors and inaccuracies in micro-architecture manuals
    * Manuals often oversimplify micro-architectural mechanisms.
    * Intricacies aren’t well documented by the vendor, and often not known.

*   Providing “speed of light” data for compiler and software optimization efforts
    * Getting it to run “as fast as the competition” may not be possible.
    * Getting it to run “as fast as possible” is a useful one.

## How do I write a new test

### Writing C code
Write test in C. Have test measure wall clock time at the beginning and end. Return a simple measure (eg: operations per second)

Here is an example test to measure the latency of dependent ALU ops.

```
struct Result dostuff1() {

  struct Result result;
  uint64_t t = now_nsec();

  for (i=0; i < LOOP1M; i++) {

    ... do stuff 1K times ...

  }

  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  strcpy(result.function, __FUNCTION__);
  result.resulthash = a;
  strcpy(result.metricname, "Stuff per second");
  return result;
}

```

### Edit cputest.h

Edit cputest.h to declare your new test function, associate your test with a
commandline flag etc

```...
struct Result dostuff1(int param);  // declare your new test function
...
...
#define NUMTESTS 25   // total number of tests. Increment this number
...
...
const struct Test alltests[NUMTESTS] = {
...
...
  {"--dostuff X",           // commandline flag that invokes your new test
   1,                       // number of parameters
   "Does stuff with parameter X",  // help info
   (struct Result (*)())dostuff1   // pointer to function that will be invoked
  },
...
...
};

```

### BUILD file
Create the appropriate targets in the BUILD file.

```
cc_library(
    name = "dostuff",
    srcs = ["dostuff.c"],
    hdrs = [
        "dostuff.h",  # if your test has a header file.
        "result.h",
        "util.h",
    ],
)
...
...
cc_binary(
    name = "cputest",
    srcs = ["cputest.c"],
    deps = [
        "alu",
        "branch",
        "dostuff",  # your new test
        "load",
        "loadstore",
        "store",
    ],
)
```
### Compiling and building the benchmarks
Building the benchmark can be configured and managed by
[Bazel](https://docs.bazel.build/versions/master/install.html).
Please note that this benchmark is only tested in Linux systems. Therefore we
cannot guarantee it would work in other systems yet.

Users external to Google can set environment variable $CC to specify which
compiler should be used to build the benchmark. However, it is strongly
suggested to use LLVM compiler. GNU tool chain is very inefficient in handling
the large inline assembly chunk inside the benchmarks. Our experiment shows that
it takes several hours to build **bigtable_analog** benchmark compared to
several minutes with LLVM.

Please compile the benchmark in debug mode (-c dbg), with no optimizations,
and force static linking (--linkopt=-static).

### Testing the test

Using traditional unit testing techniques for CPUtest can catch cerebral
flatulence on the part of the developer, but cannot get at fundamental issues in
the test. Generally the best way is to run the test on a target machine of known
frequency and of known micro-architectural characteristics (as described in
manuals). If the test produces results consistent with disclosures from the
vendor, then there is a strong likelihood that the test is correct/robust. While
modifying a test, it is good practice to check the result against the existing
MPM. There are two shell scripts that help you do that...

#### test.sh

Runs on the host machine. Copies CPUtest executable from bazel-bin directory,
copies test-on-target.sh to target machine and runs it.

```
Usage: test.sh <target machine> <cputest arguments>
```

#### test-on-target.sh

Runs on target machine. Fetches latest cputest MPM, runs it and the copied over
executable for the cputest arguments passed in.

TIP: Remember to build cputest for the correct architecture before running test.sh on a particular target machine.

Example.

```
$ bazel build --cpu=ppc --compiler=gcc -c dbg cputest
$ test.sh jktt88 "--max_scalar_store_ipc"
test-on-target.sh                                                                                                                                                                                                                                       100%  193     0.2KB/s   00:00
cputest                                                                                                                                                                                                                                                 100%   14MB  13.6MB/s   00:01
Fetched package platforms/benchmarks/microbenchmarks/cputest version latest.
/data/{{USERNAME}}/mpmtest
mpm/cputest
max_scalar_store_ipc	resulthash=1048576	GOPS=3.422395

new
max_scalar_store_ipc	resulthash=1048576	GOPS=3.424247

$
```

In this case, the local version and the MPM version produce virtually the same
result. Sometimes test result differences can be intentional. Correctness is for
the test writer to ascertain depending on the context.

## How do I run a test?
Fetch the [MPM](https://mpmbrowse.corp.google.com/packageinfoz?package=platforms/benchmarks/microbenchmarks/cputest) like this on the target machine

```
$ cd /data/{{USERNAME}}
$ mpm fetch -a platforms/benchmarks/microbenchmarks/cputest cputest
```
You can simply run it as follows (to measure ALU latency) ...

```
$ cd /data/{{USERNAME}}/cputest
$ ./cputest --dostuff1

```
### Thread and numa node binding
Often, to run a test reliably, you need to bind it to a particular logical thread using taskset or numactl. Otherwise the scheduler might migrate your test to a differnet hardware thread unexpectedly, causing your result to become suspect.

#### Using taskset
To bind cputest to logical thread 16 ....

```
$ taskset -c 16 ./cputest --dostuff1
```

#### Using numactl
numactl is a third party utility that lets you bind a task to not just a thread (or a combination thereof), but also a particular memory node. This is useful for tests that exercise the memory subsystem (eg: measuring memory bandwidth)

numactl is available as a multi-arch [MPM](https://mpmbrowse.corp.google.com/packageinfoz?package=third_party%2Flibnuma%2Fnumactl)

On the target machine, fetch the mpm as follows ...

```
$ cd /data/{{USERNAME}}
$ mpm fetch -a third_party/libnuma/numactl numactl


```
To bind the cputest run to logical core 16 and numa node 0 do....
(assuming you fetched the cputest mpm as described before)

```
$ numactl/numactl -C 16 -m 0 cputest/cputest --dostuff1
```

### Running multi-threaded tests
If you are running a test to evaluate structural hazards that come to the fore when multiple logical threads share and contend for the same resource, you can invoke cputest multiple times..

Example: If logical threads 16, 17, 18 and 19, all share the same physical core, you can run a test on all of them concurrently as follows

```
$ for i in 16 17 18 19; do
> taskset -c $i cputest/cputest --dostuff1 &
> done
$
```

NOTE: Make sure your test runs long enough so that the short duration during which only a subset of the threads are running make up only a small fraction of the total runtime.

## People

*   Trivikram Krishnamurthy: Infrastructure planning, test writing/running, documentation etc.
*   Greg Johnson (gwj@): Code review, advice on structure etc.
*   Jeremiah Willcock (jewillco@): Inline assembly help
*   Weiping Liao (weipingliao@): ARM inline assembly help

