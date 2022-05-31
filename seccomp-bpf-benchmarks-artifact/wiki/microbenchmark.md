# Microbenchmark

Time: ~4 hours. Start from the repo's root directory.

## Automatic

### Native

```bash
./seccomp-bpf-benchmarks-artifact/benchmarks/scripts/run_microbenchmark.sh
```

## Manual

### Step 1 - setting up the output for automatic processing

```bash
# Clear files containing output.
rm ./seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/*
```

### Step 2 - setting up the MVEE

```bash
cd MVEE/bin/Release/
# Disable IP-MON by editing MVEE.ini and setting "use_ipmon" to false.
sed -i "s/\"use_ipmon\" : true/\"use_ipmon\" : false/g" ./MVEE.ini
```

### Step 3 - running the experiments

```bash
# native run, do this 10 times
../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/native.out

# default ReMon, do this 10 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/default.out
```

### Step 4 - Setting up IP-MON

```bash
# Enable IP-MON by editing MVEE.ini and setting "use_ipmon" to true.
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini

# Change IP-MON policy to trace all and recompile IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_trace_all.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -

```

### Step 5 - running the experiments

```bash
# ReMon with IP-MON enabled and getpid traced, do this 10 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/ipmon_getpid_traced.out
```

### Step 6 - Reconfigure IP-MON

```bash
# Change IP-MON policy to allow getpid and recompile IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_allow_getpid.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -
```

### Step 7 - running the experiments

```bash
# ReMon with IP-MON enabled and getpid traced, do this 10 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/ipmon_getpid_allowed.out
```

### Step 8 - automatic processing

This will output the average of the runs for each buffer size for each experiment. Run this from the repo's root.

```bash
./seccomp-bpf-benchmarks-artifact/benchmarks/scripts/process_microbenchmark.sh
```