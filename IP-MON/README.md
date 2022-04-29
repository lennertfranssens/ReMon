# IP-MON
To use IP-MON, ReMon-glibc must be used. Further instructions on how to install and use IP-MON can be found in the README.md of the ReMon repository.

## Edit policy for IP-MON
IP-MON uses a seccomp-bpf filter to decide whether to handle system calls with or without IP-MON. To set the policy of the seccomp-bpf filter, you need to use a json file called seccomp_bpf_policy.json. Currently only a `default_policy` with the value `TRACE` is supported. That means that a whitelist is created for system calls that are allowed to be handled by IP-MON. The whitelist is passed as a list to the attribute `allow`. This list contains the numbers of system calls.

Below is an example of such a json file:
```json
{
    "seccomp_bpf_policy": {
        "default_policy": "TRACE",
        "allow": ["__NR_write", "__NR_writev"]
    }
}
```

The full list of system calls that is whitelisted in the old IP-MON implementation is:
```json
["__NR_uname", "__NR_getpriority", "__NR_nanosleep", "__NR_getrusage", "__NR_sysinfo", "__NR_times", "__NR_capget", "__NR_getitimer", "__NR_futex", "__NR_gettimeofday", "__NR_time", "__NR_clock_gettime", "__NR_getpid", "__NR_getegid", "__NR_geteuid", "__NR_getgid", "__NR_getpgrp", "__NR_getppid", "__NR_gettid", "__NR_getuid", "__NR_sched_yield", "__NR_getcwd", "__NR_access", "__NR_faccessat", "__NR_stat", "__NR_fstat", "__NR_lstat", "__NR_newfstatat", "__NR_getdents", "__NR_readlink", "__NR_readlinkat", "__NR_getxattr", "__NR_lgetxattr", "__NR_fgetxattr", "__NR_lseek", "__NR_alarm", "__NR_setitimer", "__NR_timerfd_gettime", "__NR_madvise", "__NR_fadvise64", "__NR_read", "__NR_pread64", "__NR_readv", "__NR_preadv", "__NR_poll", "__NR_select", "__NR_timerfd_settime", "__NR_sync", "__NR_fdatasync", "__NR_syncfs", "__NR_write", "__NR_pwrite64", "__NR_writev", "__NR_pwritev", "__NR_recvfrom", "__NR_recvmsg", "__NR_recvmmsg", "__NR_getsockname", "__NR_getpeername", "__NR_getsockopt", "__NR_sendto", "__NR_sendmsg", "__NR_sendmmsg", "__NR_sendfile", "__NR_epoll_wait", "__NR_epoll_ctl", "__NR_shutdown", "__NR_setsockopt", "__NR_ioctl"]
```