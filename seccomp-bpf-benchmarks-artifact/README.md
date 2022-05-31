# seccomp-BPF IP-MON Benchmarks Artifact

This artifact relates to the master thesis "Boosting MVX Systems Through Modern OS Extensions". All bash snippets in the steps below are assumed to start from the repository root.

## System

All benchmarks were originally run on a system running Ubuntu 20.04 LTS, equipped with a 6-core Intel® Core I5-10400 CPU,
Intel® UHD Graphics 630 iGPU, and 32GB of 2666 MHz RAM. For our experiments we disabled hyper-threading and turbo-boost.

As a rule of thumb we suggest counting one core for the monitor and n additional cores for every process that would be
started by the application being run. With n representing the configured number of variants.

## Prerequisites

For more reproducible results turn off hyper threading and turbo boost. The method for this might depend on your system.
General Intel way:

```bash
echo "1"   | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
echo "off" | sudo tee /sys/devices/system/cpu/smt/control
```

## Setup

While we suggest running the experiments in a native Ubuntu 20.04 LTS machine.

To benchmark the servers download and install wrk from https://github.com/wg/wrk. Clone the wrk repository, run `make` in the repository root directory and move the wrk executable to your PATH with `sudo cp wrk /usr/local/bin`. To reproduce our results, run the web server on one
system and the wrk benchmark on a second system, connecting both through a dedicated gigabit ethernet connection.

## Native execution instructions

### Step 1 - bootstrap

```bash
BUILDALL=1 ./seccomp-bpf-benchmarks-artifact/bootstrap.sh
```

### Step 2 - build custom glibc

```bash
cd deps/
git clone git@github.com:lennertfranssens/ReMon-glibc.git
cd ReMon-glibc/
git checkout ipmon_48_bit_secret
mkdir build
cd build/
../configure-libc-woc.sh
make -j$(nproc)
make install
cd ../../../
cp ~/glibc-build/lib/libc-2.31.so patched_binaries/libc/amd64/libc-2.31.so
```

### Step 3 - run experiments

**Microbenchmark**: follow instructions in seccomp-bpf-benchmarks-artifact/wiki/microbenchmark.md

**Nginx**: follow instructions in seccomp-bpf-benchmarks-artifact/wiki/nginx.md
