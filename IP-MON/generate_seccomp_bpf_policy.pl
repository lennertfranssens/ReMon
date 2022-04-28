#!/usr/bin/env perl
# sudo apt-get install -y libjson-perl
use JSON;
use Data::Dumper;

my $address = 'seccomp_bpf_policy.json';
my $json = do {                           # do block to read file into variable
    local $/;                             # slurp entire file
    open(FH, "<", $address) or die $!;    # open for reading
    <FH>;                                 # read and return file content
};
close(FH);

#print Dumper($json);
my $text = decode_json($json);
#print Dumper($text);

$filename = 'MVEE_ipmon_seccomp_bpf_policy.h';
open(FH, '>', $filename) or die $!;

my $str = "
int A = 1,
B = 2,
C = 11,
D = 13,
E = 23;

// Define empty BPF-filter while starting variant
struct sock_filter filter[] = {
/* [0] Load the instruction pointer from 'seccomp_data' buffer into accumulator */
BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, instruction_pointer))),
/* [1][A] Jump forward to the next if instruction pointer does not match 
the ipmon checked syscall instruction address. If it matches, we need to trace (E-A-1) */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ((__u32*)&ipmon_checked_syscall_instr_ptr)[0], (unsigned char)(E-A-1), 0),
/* [2][B] Jump forward C-A-1 instructions if instruction pointer does not match 
the ipmon unchecked syscall instruction address. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ((__u32*)&ipmon_unchecked_syscall_instr_ptr)[0], 0, (unsigned char)(D-B-1)),
/* [3] Load system call number from 'seccomp_data' buffer into accumulator. */
BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),
/* [4-10] Jump forward 0 instructions if system call number does not match '__NR_XXX'. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_futex, 7, 0), // futex coming from ipmon_enclave is allowed to guarantee synchronization
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_sched_yield, 6, 0), // sched_yield coming from ipmon_enclave is allowed to guarantee synchronization
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 5, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 4, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 3, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 2, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 1, 0),
/* [11][C] Jump forward E-C-1 instructions if system call number does not match '__NR_XXX'. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 0, (unsigned char)(E-C-1)), // TODO: Change back to __NR_getuid
/* [12] Execute the system call */
BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW), // SECCOMP_RET_TRACE to check if the filter works
/* [13][D] Load system call number from 'seccomp_data' buffer into accumulator. */
BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),
/* [14-20] Jump forward 0 instructions if system call number does not match '__NR_XXX'. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 7, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 6, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 5, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 4, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 3, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 2, 0),
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 1, 0),
/* [21] Jump forward 1 instructions if system call number does not match '__NR_XXX'. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_write, 0, 1), // TODO: Change back to __NR_getuid
/* [22] Return the ipmon syscall entry address */
BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (ipmon_enclave_entrypoint_ptr_in_16_bits & SECCOMP_RET_DATA)),
/* [23][E] Execute the system call in tracer */
BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),

//BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
};";

print FH $str;

close(FH);

print "MVEE_ipmon_seccomp_bpf_policy.h generated\n";