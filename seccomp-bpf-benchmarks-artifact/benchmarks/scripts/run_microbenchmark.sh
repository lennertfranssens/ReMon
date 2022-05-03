#!/bin/bash
set -e

cd "$(readlink -f $(dirname ${BASH_SOURCE})/../../../)"

# Clear files containing output.
rm ./eurosys2022-artifact/benchmarks/results/microbenchmark/* || :


# Optional for when you want to enable IP-MON, has no effect when kernel is not IP-MON enabled.
cd IP-MON/
ln -fs libipmon-default.so libipmon.so
cd ../


cd MVEE/bin/Release/
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini


# Native run
echo " > running native microbenchmark..."
for __i in {1..5}
do
    echo " > run $__i/5"
    ../../../eurosys2022-artifact/benchmarks/microbenchmark/getpid >> ../../../eurosys2022-artifact/benchmarks/results/microbenchmark/native.out
done

../../../eurosys2022-artifact/benchmarks/scripts/relink_glibc.sh default
echo " > running mvee microbenchmark with wrapped access..."
for __i in {1..5}
do
    echo " > run $__i/5"
    ./mvee -N 1 -- ../../../eurosys2022-artifact/benchmarks/microbenchmark/getpid >> ../../../eurosys2022-artifact/benchmarks/results/microbenchmark/default.out
done

# Make sure the correct libc version is used for later experiments.
../../../eurosys2022-artifact/benchmarks/scripts/relink_glibc.sh default


# Output result.
../../../eurosys2022-artifact/benchmarks/scripts/process_microbenchmark.sh
