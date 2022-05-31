# Nginx

Time required: ~10 minutes.  Start from the repo's root directory.

## Step 0 - setup

Open two terminal windows: 
- server: nginx is started from this terminal
- client: this terminal runs wrk

Ideally, the client terminal is opened on a separate machine connected via a dedicated gigabit ethernet link, to replicate our evaluation setup.

Client wrk command: `wrk -d 10s -t 1 -c 10 --timeout 10s http://127.0.0.1:8080 >> outfile`, outfile is mentioned in the comments between the commands. If you are using separate machines to benchmark nginx, the ip is the ip on the dedicated link for the machine running nginx.

## Step 1 - setting up the MVEE

```bash
cd MVEE/bin/Release/
# Disable IP-MON by editing MVEE.ini and setting "use_ipmon" to false.
sed -i "s/\"use_ipmon\" : true/\"use_ipmon\" : false/g" ./MVEE.ini
```

## Step 2 - running the experiments

```bash
# native run, 1 worker, do this 5 times
../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx
# run the wrk command with "nginx-native-1worker" as outfile in the other terminal and wait for the results
../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx -s stop

# default ReMon run, 1 workers, do this 5 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx
# run the wrk command with "nginx-default-ReMon-1worker" as outfile in the other terminal and wait for the results
# crtl+c to terminate the MVEE
```

## Step 3 - Setting up IP-MON

```bash
# Enable IP-MON by editing MVEE.ini and setting "use_ipmon" to true.
sed -i "s/\"use_ipmon\" : false/\"use_ipmon\" : true/g" ./MVEE.ini

# Change IP-MON policy to trace all and recompile IP-MON
cp ../../../seccomp-bpf-benchmarks-artifact/benchmarks/patches/IP-MON/seccomp_bpf_policy_nginx.json ../../../IP-MON/seccomp_bpf_policy.json
cd ../../../seccomp-bpf-benchmarks-artifact/
./comp-ipmon.sh
cd -
```

## Step 4 - running the experiments

```bash
# ReMon with IP-MON run, 2 workers, do this 5 times
./mvee -N 1 -- ../../../seccomp-bpf-benchmarks-artifact/benchmarks/out/nginx/default/sbin/nginx
# run the wrk command with "nginx-ReMon-IP-MON-1worker" as outfile in the other terminal and wait for the results
# crtl+c to terminate the MVEE
```

## Step 5 - automatic processing

If you used a separate client machine, copy all outfiles generated to seccomp-bpf-benchmarks-artifact/benchmarks/results/nginx.

This will output the average of the runs for each experiment. This does not have to be run inside the docker container,
but works either way. Run this from the repo's root.

```bash
./seccomp-bpf-benchmarks-artifact/benchmarks/scripts/process_nginx.sh
```