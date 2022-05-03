#!/bin/bash
set -e

__home_dir="$(readlink -f $(dirname ${BASH_SOURCE})/../../)"
cd "$__home_dir/../patched_binaries/libc/amd64/"


case "$1" in
  ipmon)
    echo " > setting a ipmon version of our libc"
    ln -fs "$__home_dir/../deps/ReMon-glibc/build/built-versions/ipmon/"* \
      "$__home_dir/../patched_binaries/libc/amd64"
    break
    ;;
  default)
    echo " > setting the default version of our libc"
    ln -fs "$__home_dir/../deps/ReMon-glibc/build/built-versions/normal/"* \
      "$__home_dir/../patched_binaries/libc/amd64"
    break
    ;;
  *)
    echo " > unrecognized option $1, setting default version of our libc instead"
    ln -fs "$__home_dir/../deps/ReMon-glibc/build/built-versions/normal/"* \
      "$__home_dir/../patched_binaries/libc/amd64"
    break
    ;;
esac
