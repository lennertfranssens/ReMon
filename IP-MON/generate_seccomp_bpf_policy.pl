#!/usr/bin/env perl
# install libjson-perl for JSON module
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
my $config = decode_json($json);
#print Dumper($config);

if ($config->{"seccomp_bpf_policy"}{"default_policy"} eq 'TRACE') {
  $filename = 'MVEE_ipmon_seccomp_bpf_policy.h';
  open(FH, '>', $filename) or die $!;

  my $variables =
"// Define variables for jump instructions in the seccomp-bpf filter
";
  my $filter =
"// Define seccomp-bpf filter
struct sock_filter filter[] = {
";

  my @allowed_syscalls = @{$config->{"seccomp_bpf_policy"}{"allow"}};

  if (@allowed_syscalls == 0) {
        $filter .=
"BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),
";
  } else {
    my @always_allowed_syscalls = ("__NR_futex", "__NR_sched_yield");
    push(@always_allowed_syscalls, @allowed_syscalls);

    my $A = 1, $B = 2, $C, $D, $E;

    my $line = "";

    $filter .= 
"/* [0] Load the instruction pointer from 'seccomp_data' buffer into accumulator */
BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, instruction_pointer))),
/* [1][A] Jump forward to the next if instruction pointer does not match the ipmon checked syscall instruction address. If it matches, we need to trace (E-A-1) */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ((__u32*)&ipmon_checked_syscall_instr_ptr)[0], (unsigned char)(E-A-1), 0),
/* [2][B] Jump forward C-A-1 instructions if instruction pointer does not match the ipmon unchecked syscall instruction address. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ((__u32*)&ipmon_unchecked_syscall_instr_ptr)[0], 0, (unsigned char)(D-B-1)),
/* [3] Load system call number from 'seccomp_data' buffer into accumulator. */
BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),
";

    $C = @always_allowed_syscalls + 3;
    $D = $C + 2;

    $line =
"/* [4-" . ($C - 1) . "] Jump forward 0 instructions if system call number does not match '__NR_XXX'. */
";
    $filter .= $line;

    my $i = @always_allowed_syscalls - 1;

    while ($i >= 1) {
        $line =
"BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, " . $always_allowed_syscalls[$i] . ", " . $i . ", 0),
";
        $filter .= $line;
        $i--;
    }

    $i = $C + 1;

    $line =
"/* [" . $C . "][C] Jump forward E-C-1 instructions if system call number does not match '__NR_XXX'. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, " . $always_allowed_syscalls[0] . ", 0, (unsigned char)(E-C-1)),
/* [" . $i++ . "] Execute the system call */
BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW), // SECCOMP_RET_TRACE to check if the filter works
/* [" . $D . "][D] Load system call number from 'seccomp_data' buffer into accumulator. */
BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),
";
    $filter .= $line;

    my $i = @allowed_syscalls - 1;

    if ($i > 0) {
      $line =
"/* [" . ($D + 1) . "-" . ($D + @allowed_syscalls) . "] Jump forward 0 instructions if system call number does not match '__NR_XXX'. */
";
      $filter .= $line;

      while ($i >= 1) {
          $line =
"BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, " . $allowed_syscalls[$i] . ", " . $i . ", 0),
";
          $filter .= $line;
          $i--;
      }
    }

    $i = $D + @allowed_syscalls;

    $line =
"/* [" . $i++ . "] Jump forward 1 instructions if system call number does not match '__NR_XXX'. */
BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, " . $allowed_syscalls[0] . ", 0, 1),
/* [" . $i++ . "] Return the ipmon syscall entry address */
BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (ipmon_enclave_entrypoint_ptr_in_16_bits & SECCOMP_RET_DATA)),
/* [" . $i++ . "][E] Execute the system call in tracer */
BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),
";
    $filter .= $line;

    $E = $i - 1;

    $variables .=
"int A = " . $A . ",
B = " . $B . ",
C = " . $C . ",
D = " . $D . ",
E = " . $E . ";
";

  }

  $filter .=
"};
";
  print FH $variables;
  print FH $filter;

  close(FH);

  print "MVEE_ipmon_seccomp_bpf_policy.h generated\n";

} else {
  print "We only support the TRACE default_policy for now...";
}
