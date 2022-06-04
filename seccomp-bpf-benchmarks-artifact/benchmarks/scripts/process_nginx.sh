#!/bin/bash
set -e

cd "$(readlink -f $(dirname ${BASH_SOURCE})/../results/nginx/)"

echo " > nginx native run, 1 worker"
python ../../scripts/process_wrk.py "nginx-native-1worker"

echo " > nginx default ReMon run, 1 worker"
python ../../scripts/process_wrk.py "nginx-default-ReMon-1worker"

echo " > nginx ReMon with IP-MON run, 1 worker"
python ../../scripts/process_wrk.py "nginx-ReMon-IP-MON-1worker"

echo " > nginx ReMon with old IP-MON with kernel patch, 1 worker"
python ../../scripts/process_wrk.py "nginx-IP-MON-kernel-1worker"
