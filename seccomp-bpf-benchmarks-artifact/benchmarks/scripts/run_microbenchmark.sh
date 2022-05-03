#!/bin/bash
set -e

cd "$(readlink -f $(dirname ${BASH_SOURCE})/../../../)"

# Clear files containing output.
rm ./seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/* || :


# Optional for when you want to enable IP-MON, has no effect when kernel is not IP-MON enabled.
cd IP-MON/
ln -fs libipmon-default.so libipmon.so
cd ../

# Do not use IP-MON for default run
cd MVEE/bin/Release/
sed -i "s/\"use_ipmon\" : true/\"use_ipmon\" : false/g" ./MVEE.ini


# Native run
echo " > running native microbenchmark..."
for __i in {1..5}
do
    echo " > run $__i/5"
    ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/native.out
done

# Default ReMon run
../../../seccomp-bpf-benchmarks-artifact/benchmarks/scripts/relink_glibc.sh default
echo " > running mvee microbenchmark with default ReMon..."
for __i in {1..5}
do
    echo " > run $__i/5"
    ./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/default.out
done

# ReMon with IP-MON enabled and getpid traced run
../../../seccomp-bpf-benchmarks-artifact/benchmarks/scripts/relink_glibc.sh ipmon
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_trace_all.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini
echo " > running mvee microbenchmark with ReMon with IP-MON enabled and getpid traced..."
for __i in {1..5}
do
    echo " > run $__i/5"
    ./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/ipmon_getpid_traced.out
done

# ReMon with IP-MON enabled and getpid allowed run
../../../seccomp-bpf-benchmarks-artifact/benchmarks/scripts/relink_glibc.sh ipmon
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_allow_getpid.json ../../../IP-MON/seccomp_bpf_policy.json

cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini
echo " > running mvee microbenchmark with ReMon with IP-MON enabled and getpid allowed..."
for __i in {1..5}
do
    echo " > run $__i/5"
    ./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/ipmon_getpid_allowed.out
done

# Make sure the correct libc version is used for later experiments.
../../../seccomp-bpf-benchmarks-artifact/benchmarks/scripts/relink_glibc.sh default

# Trace all syscalls in IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_trace_all.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -

# Do not use IP-MON in later experiments
sed -i "s/\"use_ipmon\" : true/\"use_ipmon\" : false/g" ./MVEE.ini

# Output result.
../../../seccomp-bpf-benchmarks-artifact/benchmarks/scripts/process_microbenchmark.sh
