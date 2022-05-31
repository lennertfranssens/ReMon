#!/bin/bash
set -e

__home_dir=$(readlink -f $(dirname ${BASH_SOURCE}))


$__home_dir/nginx_build.sh \
	--base		   \
        --default
