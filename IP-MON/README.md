# IP-MON
To use IP-MON, ReMon-glibc must be used. Further instructions on how to install and use IP-MON can be found in the README.md of the ReMon repository.

## Edit policy for IP-MON
IP-MON uses a seccomp-bpf filter to decide whether to handle system calls with or without IP-MON. To set the policy of the seccomp-bpf filter, you need to use a json file called seccomp_bpf_policy.json. Currently only a `default_policy` with the value `TRACE` is supported. That means that a whitelist is created for system calls that are allowed to be handled by IP-MON. The whitelist is passed as a list to the attribute `allow`. This list contains the numbers of system calls.

Below is an example of such a json file:
```json
{
    "seccomp_bpf_policy": {
        "default_policy": "TRACE",
        "allow": ["__NR_write", "__NR_write"]
    }
}
```