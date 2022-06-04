#!/bin/bash
set -e

__home_dir="$(readlink -f $(dirname ${BASH_SOURCE}))"
cd "$__home_dir"

sudo apt update
sudo apt install -y libpulse-dev libxv-dev libxext-dev libx11-dev libx11-xcb-dev libxtst-dev libfreetype6-dev    \
        libfontconfig-dev gperf libpcre3-dev libexpat1-dev autopoint libtool libtool-bin libsndfile1-dev gettext \
        libssl-dev python libice-dev libsm-dev uuid-dev gcc binutils libiberty-dev unzip gawk

# ReMon Setup #########################################################################################################
cd ../
./bootstrap.sh
cd ./build/

make benchmark
make ipmon_compatible
make -j$(nproc)

sudo apt install -y clang perl libjson-perl
cd "$__home_dir/../seccomp-bpf-benchmarks-artifact/"
./comp-ipmon.sh
##########################################################################

## Benchmark Setup #####################################################################################################
cd "$__home_dir/benchmarks/"

if [ ! -d "$__home_dir/benchmarks/nginx/" ]
then
    cd "$__home_dir/benchmarks/"
    wget http://nginx.org/download/nginx-1.18.0.tar.gz
    tar -xf nginx-1.18.0.tar.gz
    mv nginx-1.18.0 nginx
    patch -d ./nginx/ -p 2 < ./patches/nginx.patch
    cp "$__home_dir/benchmarks/conf/nginx.conf" "$__home_dir/benchmarks/nginx/conf/"
    cp "$__home_dir/benchmarks/input/index.html" "$__home_dir/benchmarks/nginx/html/"
fi

cd "$__home_dir/benchmarks/microbenchmark/"
make
cd ../

echo "__llvm_dir=\"$__home_dir/../deps/llvm/build-tree/\"" > "$__home_dir/benchmarks/"/config.sh
if [[ "$BUILDALL" == 1 ]]
then
    "$__home_dir/benchmarks/scripts/build_all.sh"
fi

cd "$__home_dir/"
mkdir -p ./benchmarks/results/microbenchmark
## Benchmark Setup #####################################################################################################
