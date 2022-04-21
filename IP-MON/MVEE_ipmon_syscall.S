/*
 * GHent University Multi-Variant Execution Environment (GHUMVEE)
 *
 * This source file is distributed under the terms and conditions 
 * found in IPMONLICENSE.txt.
 */

/*-----------------------------------------------------------------------------
    Convenience Macros
-----------------------------------------------------------------------------*/
#define FUNCTION_START(name)			\
		.globl name;					\
		.type name,@function;			\
		.hidden name;					\
		.cfi_startproc;					\
		.align 16;						\
name:

#define FUNCTION_END(name);             \
		.cfi_endproc;					\
		.size name,.-name

#define FUNCTION_START_EXPORTED(name)   \
		.globl name;                    \
		.type name,@function;           \
		.cfi_startproc;                 \
		.align 16;                      \
name:

/*-----------------------------------------------------------------------------
    Global Variables
-----------------------------------------------------------------------------*/
		.section .data
		.globl ipmon_initialized
ipmon_initialized:
		.byte 0


		.section .text
		.intel_syntax noprefix


/*-----------------------------------------------------------------------------
	ipmon_unchecked_syscall_instr - Syscall instruction that is executed when 
	ipmon_unchecked_syscall is executed.

TODO:	 This function should be inlined in the C code!!
-----------------------------------------------------------------------------*/
FUNCTION_START(ipmon_unchecked_syscall_instr)
		syscall
		ret
FUNCTION_END(ipmon_unchecked_syscall_instr)

/*-----------------------------------------------------------------------------
	ipmon_checked_syscall_instr - Syscall instruction that is executed when 
	ipmon_checked_syscall is executed.

TODO:	 This function should be inlined in the C code!!
-----------------------------------------------------------------------------*/
FUNCTION_START(ipmon_checked_syscall_instr)
		syscall
		ret
FUNCTION_END(ipmon_checked_syscall_instr)

/*-----------------------------------------------------------------------------
	ipmon_unchecked_syscall - This is where we land if the syscall does not
	need to go to he ptracer. The seccomp bpf filter knows the instruction
	pointer of the unchecked syscall address so it evaluates to return a
	SECCOMP_RET_ALLOW action.

TODO:	 This function should be inlined in the C code!!
-----------------------------------------------------------------------------*/
FUNCTION_START(ipmon_unchecked_syscall)
		mov rax, rdi
		mov rdi, rsi
		mov rsi, rdx
		mov rdx, rcx
		mov r10, r8
		mov r8, r9
		mov r9, QWORD PTR [rsp + 8]
		call ipmon_unchecked_syscall_instr@plt
		ret
FUNCTION_END(ipmon_unchecked_syscall)

/*-----------------------------------------------------------------------------
	ipmon_checked_syscall - This is where we land if the syscall must go to
	the ptracer. The seccomp bpf filter knows the instruction pointer of the
	checked syscall address so it evaluates to return a SECCOMP_RET_TRACE action.

TODO:	This function should be inlined in the C code!!
-----------------------------------------------------------------------------*/
FUNCTION_START(ipmon_checked_syscall)
		mov rax, rdi
		mov rdi, rsi
		mov rsi, rdx
		mov rdx, rcx
		mov r10, r8
		mov r8, r9
		mov r9, QWORD PTR [rsp + 8]
		
		call ipmon_checked_syscall_instr@plt
		
		ret
FUNCTION_END(ipmon_checked_syscall)

/*-----------------------------------------------------------------------------
    ipmon_enclave_entrypoint - This is where the kernel lands when the app
	executes a syscall that is on the IP-MON whitelist. Incoming arguments are
	as follows:

rax:	original syscall number
rdi:	arg1
rsi:	arg2
rdx:	arg3
r10:	arg4
r8:		arg5
r9:		arg6

	In this function, we just do some juggling so we can handle all of the
	replication/checking logic in plain C code.

	Outgoing args:

rdi:	original syscall no
rsi:	arg1
rdx:	arg2
rcx:	arg3
r8:		arg4
r9:		arg5

Outgoing Stack:
		+00 enclave entrypoint return address
-----------------------------------------------------------------------------*/
		.align 4096									// algin 4096 to get the 12 most lsbs to 0
FUNCTION_START_EXPORTED(ipmon_enclave_entrypoint)
		// The callee (ipmon_enclave) will preserve registers rbp,rbx and r12-r15.
		// We are responsible for preserving the rest of the registers except for:
		// rcx,r11 because the kernel is allowed to clobber these
		// rax because this is our syscall return value
		// We must preserve: rdx, rsi, r8, r9, r10, rdi, rsp		

		push rbp
		.cfi_adjust_cfa_offset 8
		push rdx
		.cfi_adjust_cfa_offset 8
		push rsi
		.cfi_adjust_cfa_offset 8
		push r10
		.cfi_adjust_cfa_offset 8
		push rdi
		.cfi_adjust_cfa_offset 8
		mov rbp, rsp
		.cfi_def_cfa_register rbp

		// We need to align the stack
		sub rsp, 16
		and rsp, 0xfffffffffffffff0
		sub rsp, 8

		push r9  // arg6

		mov r9, r8   // arg5
		mov r8, r10  // arg4
		mov rcx, rdx // arg3
		mov rdx, rsi // arg2
		mov rsi, rdi // arg1
		mov rdi, rax // syscall no
		
		call ipmon_enclave@plt

		pop r9
		mov rsp, rbp
		.cfi_def_cfa_register rbp
		pop rdi
		.cfi_adjust_cfa_offset -8
		pop r10
		.cfi_adjust_cfa_offset -8
		pop rsi
		.cfi_adjust_cfa_offset -8
		pop rdx
		.cfi_adjust_cfa_offset -8
		pop rbp
		.cfi_adjust_cfa_offset -8

		retq
		
FUNCTION_END(ipmon_enclave_entrypoint)
