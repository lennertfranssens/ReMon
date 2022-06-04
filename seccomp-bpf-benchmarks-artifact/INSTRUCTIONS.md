# Running the experiments

All command listings start from the repository's root.

## microbenchmark

```bash
# prints out: "> size: time in ns"

cd MVEE/bin/Release/
# Disable IP-MON by editing MVEE.ini and setting "use_ipmon" to false.
sed -i "s/\"use_ipmon\" : true/\"use_ipmon\" : false/g" ./MVEE.ini

# native run, do this 10 times
../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/native.out

# default ReMon, do this 10 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/default.out

# Enable IP-MON by editing MVEE.ini and setting "use_ipmon" to true.
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini

# Change IP-MON policy to trace all and recompile IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_trace_all.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -

# ReMon with IP-MON enabled and getpid traced, do this 10 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/ipmon_getpid_traced.out

# Change IP-MON policy to allow getpid and recompile IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_allow_getpid.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -

# ReMon with IP-MON enabled and getpid traced, do this 10 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/microbenchmark/getpid >> ../../../seccomp-bpf-benchmarks-artifact/benchmarks/results/microbenchmark/ipmon_getpid_allowed.out
```

## nginx

Open two terminal windows: one to run nginx from and one to run wrk from. Ideally, the one running wrk is opened on a
separate machine connected via a dedicated gigabit ethernet link, to replicate our evaluation setup.

wrk command: `wrk -d 10s -t 1 -c 10 --timeout 10s http://127.0.0.1:8080`. If you are using separate machines to
benchmark nginx, the ip is the ip on the dedicated link for the machine running nginx.

```bash
cd MVEE/bin/Release/
# Disable IP-MON by editing MVEE.ini and setting "use_ipmon" to false.
sed -i "s/\"use_ipmon\" : true/\"use_ipmon\" : false/g" ./MVEE.ini

# native run, 1 worker, do this 5 times
../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx
# run the wrk command with "nginx-native-1worker" as outfile in the other terminal and wait for the results
../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx -s stop

# default ReMon run, 1 workers, do this 5 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx
# run the wrk command with "nginx-default-ReMon-1worker" as outfile in the other terminal and wait for the results
# crtl+c to terminate the MVEE

# Enable IP-MON by editing MVEE.ini and setting "use_ipmon" to true.
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini

# Change IP-MON policy to trace all and recompile IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_nginx.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -

# ReMon with IP-MON run, 2 workers, do this 5 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx
# run the wrk command with "nginx-ReMon-IP-MON-1worker" as outfile in the other terminal and wait for the results
# crtl+c to terminate the MVEE
```
