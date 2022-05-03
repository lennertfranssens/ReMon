#!/bin/bash
set -e

__home_dir="$(readlink -f $(dirname ${BASH_SOURCE}))"
cd "$__home_dir"

sudo apt update
sudo apt install -y libpulse-dev libxv-dev libxext-dev libx11-dev libx11-xcb-dev libxtst-dev libfreetype6-dev    \
        libfontconfig-dev gperf libpcre3-dev libexpat1-dev autopoint libtool libtool-bin libsndfile1-dev gettext \
        libssl-dev python libice-dev libsm-dev uuid-dev gcc binutils libiberty-dev unzip gawk

## ReMon Setup #########################################################################################################
cd ../
./bootstrap.sh
cd ./build/

make emulate-shm
make benchmark
make -j$(nproc)
make emulate-shm
make debug
make -j$(nproc)

sudo apt install -y clang perl libjson-perl
cd ../IP-MON
ruby ./generate_headers.rb

#cp "$__home_dir/benchmarks/patches/IP-MON/"* "$__home_dir/../IP-MON/"

./comp.sh
mv ./libipmon.so ./libipmon-default.so

#patch -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-nginx.patch
#./comp.sh
#mv ./libipmon.so ./libipmon-nginx.so
#patch -R -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-nginx.patch

#patch -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-apache.patch
#./comp.sh
#mv ./libipmon.so ./libipmon-apache.so
#patch -R -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-apache.patch

#patch -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-mplayer.patch
#./comp.sh
#mv ./libipmon.so ./libipmon-mplayer.so
#patch -R -p 2 -d ./ < ../eurosys2022-artifact/benchmarks/patches/ipmon-mplayer.patch

ln -fs ./libipmon-default.so ./libipmon.so
## ReMon Setup #########################################################################################################


## glibc Setup #########################################################################################################
cd "$__home_dir/../deps/"
if [ ! -d "./ReMon-glibc/" ]
then
    git clone https://github.com/lennertfranssens/ReMon-glibc.git
    cd "./ReMon-glibc/"
    mkdir "./build/"
    cd "./build/"
    mkdir "built-versions/"
    mkdir "built-versions/normal/"

    # really dangerous config for u. Never, EVER, do make install with this.
    CFLAGS="-O2 -fno-builtin" ../configure --enable-stackguard-randomization --enable-obsolete-rpc --enable-pt_chown \
        --with-selinux --enable-lock-elision=no --enable-addons=nptl --prefix=/ --sysconfdir=/etc/

    git checkout ipmon
    make -j$(nproc)
    cp "./elf/ld.so"             "./built-versions/normal/"
    cp "./libc.so.6"             "./built-versions/normal/"
    cp "./dlfcn/libdl.so.2"      "./built-versions/normal/"
    cp "./math/libm.so.6"        "./built-versions/normal/"
    cp "./nptl/libpthread.so.0"  "./built-versions/normal/"
    cp "./resolv/libresolv.so.2" "./built-versions/normal/"
    cp "./rt/librt.so.1"         "./built-versions/normal/"
    cp "./login/libutil.so.1"    "./built-versions/normal/"

    ln -fs "$__home_dir/../deps/ReMon-glibc/build/built-versions/normal/"* \
		"$__home_dir/../patched_binaries/libc/amd64"
fi
## glibc Setup #########################################################################################################


## Benchmark Setup #####################################################################################################
cd "$__home_dir/benchmarks/"

cd "$__home_dir/benchmarks/microbenchmark/"
make
cd ../

echo "__llvm_dir=\"$__home_dir/../deps/llvm/build-tree/\"" > "$__home_dir/benchmarks/"/config.sh
#if [[ "$BUILDALL" == 1 ]]
#then
#    "$__home_dir/benchmarks/scripts/build_all.sh"
#fi

cd "$__home_dir/"
mkdir -p ./benchmarks/results/microbenchmark
## Benchmark Setup #####################################################################################################
