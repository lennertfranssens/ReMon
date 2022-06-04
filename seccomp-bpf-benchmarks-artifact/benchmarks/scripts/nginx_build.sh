#!/bin/bash
set -e


cd "$(readlink -f $(dirname ${BASH_SOURCE})/../nginx/)"


. ../config.sh
__llvm_bin_dir="$__llvm_dir/bin/"
if [ ! -d "$__llvm_bin_dir" ]
then
  echo " > please set llvm bin directory path to valid path"
  echo " > current: $__llvm_bin_dir"
  exit 1
fi

__current_dir=$(pwd)

do_make () 
{
  make clean || :
  make distclean || :
  ./configure --prefix="$__prefix" --with-http_ssl_module
  make -j"$(nproc)" install
  ln -fs "$__current_dir/conf/nginx.conf" "$__prefix/conf/"
}

# Common flags: Wrap atomics
CFLAGS="-g -O3 -fatomicize"

while test $# -gt 0
do
  case $1 in
    --base)
      __prefix="$__current_dir/../out/nginx/base"

      export CC="$__llvm_bin_dir/clang -g -O3"
      export CXX="$__llvm_bin_dir/clang++ -g -O3"

      do_make

      shift
      ;;

    --default)
      __prefix="$__current_dir/../out/nginx/default"

      export CC="$__llvm_bin_dir/clang $CFLAGS"
      export CXX="$__llvm_bin_dir/clang++ $CFLAGS"

      do_make

      shift
      ;;

    *)
      echo " > unrecognised option $1"
      exit 1
  esac
done

