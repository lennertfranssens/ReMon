#!/bin/bash
set -e

cd "$(readlink -f $(dirname ${BASH_SOURCE})/../results/microbenchmark/)"

process ()
{
    sed -ni "/.*>.*:.*ns/p" $1
    python ../../scripts/process_microbenchmark.py $1
}

echo " > native run"
process "native.out"
echo " > default mvee"
process "default.out"
echo " > ipmon enabled mvee with getpid traced"
process "ipmon_getpid_traced.out"
echo " > ipmon enabled mvee with getpid allowed"
process "ipmon_getpid_allowed.out"
