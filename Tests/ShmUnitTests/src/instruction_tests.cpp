//
// Created by jonas on 05.06.20.
//

#include "instruction_testing.h"
#include "instruction_tests.h"
#include "stats.h"
#include "buffers.h"

// =====================================================================================================================
//      instruction tests
// =====================================================================================================================
void            instruction_tests::test_0x01                        ()
{
    START_TEST("Running tests for add m(16, 32, 64), r(16, 32, 64)...\n")

    unsigned long long original = 0x1111111111111111;
    unsigned long long sum = 2 * original;

    unsigned long long original_overflow = 0xffffffffffffffff;
    unsigned long long sum_overflow = original_overflow + 1;

    unsigned long long flags;

    // 64-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "add QWORD PTR [%[dst]], %[src];"
            "pushf;"
            "pop %[flags];"
            ".att_syntax;"
            : [flags] "+r" (flags)
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
            : "memory"
    );
    TEST_RESULT("add m64, r64 | 0x1111111111111111 * 2",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &sum, QWORD_SIZE) == 0 &&
            !(flags & CF_MASK));

    *(unsigned long long*) buffers::shared_sink = original_overflow;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "add QWORD PTR [%[dst]], %[src];"
            "pushf;"
            "pop %[flags];"
            ".att_syntax;"
            : [flags] "+r" (flags)
            : [dst] "r" (buffers::shared_sink), [src] "r" ((unsigned long long) 1)
            : "memory"
    );
    TEST_RESULT("add m64, r64 | overflow",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &sum_overflow, QWORD_SIZE) == 0 &&
            flags & CF_MASK);
    // 64-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "add DWORD PTR [%[dst]], %[src];"
            "pushf;"
            "pop %[flags];"
            ".att_syntax;"
            : [flags] "+r" (flags)
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint32_t) original)
            : "memory"
    );
    TEST_RESULT("add m32, r32 | 0x11111111 * 2",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &sum, DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE,
                    ((__uint8_t*) &original) + DWORD_SIZE, DWORD_SIZE) == 0 &&
            !(flags & CF_MASK));

    *(unsigned long long*) buffers::shared_sink = original_overflow;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "add DWORD PTR [%[dst]], %[src];"
            "pushf;"
            "pop %[flags];"
            ".att_syntax;"
            : [flags] "+r" (flags)
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint32_t) 1)
            : "memory"
    );
    TEST_RESULT("add m32, r32 | overflow",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &sum_overflow, DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE,
                    ((__uint8_t*) &original_overflow) + DWORD_SIZE, DWORD_SIZE) == 0 &&
            flags & CF_MASK);
    // 32-bit ----------------------------------------------------------------------------------------------------------

    // 16-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "add WORD PTR [%[dst]], %[src];"
            "pushf;"
            "pop %[flags];"
            ".att_syntax;"
            : [flags] "+r" (flags)
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint16_t) original)
            : "memory"
    );
    TEST_RESULT("add m16, r16 | 0x1111 * 2",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &sum, WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE,
                    ((__uint8_t*) &original) + WORD_SIZE, 3 * WORD_SIZE) == 0 &&
            !(flags & CF_MASK));

    *(unsigned long long*) buffers::shared_sink = original_overflow;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "add WORD PTR [%[dst]], %[src];"
            "pushf;"
            "pop %[flags];"
            ".att_syntax;"
            : [flags] "+r" (flags)
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint16_t) 1)
            : "memory"
    );
    TEST_RESULT("add m16, r16 | overflow",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &sum_overflow, WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE,
                    ((__uint8_t*) &original_overflow) + WORD_SIZE, 3 * WORD_SIZE) == 0 &&
            flags & CF_MASK);
    // 16-bit ----------------------------------------------------------------------------------------------------------

    FINISH_TEST("add tests successful!", "add tests failed!");
}


void            instruction_tests::test_0x39                        ()
{
    START_TEST("Running tests for cmp (0x39)\n");


    unsigned long long flags;
    unsigned long long original_src = 0xf0f0f0f0f0f0f0f0;
    unsigned long long original_dst = 0x0f0f0f0f0f0f0f0f;
    unsigned long long source;

    // 64-bit ----------------------------------------------------------------------------------------------------------
    source = original_src;
    *(unsigned long long*) buffers::shared_sink = original_src;
    flags = 0;
    __asm(
            ".intel_syntax noprefix;"
            "pushf;"
            "push rcx;"
            "popf;"
            "cmp QWORD PTR [rax], rdx;"
            "pushf;"
            "pop rcx;"
            "popf;"
            ".att_syntax;"
            : "+c" (flags)
            : "a" (buffers::shared_sink), "d" (source), "m" (buffers::shared_sink)
    );
    TEST_RESULT("cmp m64, r64 - equal", (flags & ZF_MASK))

    source = original_src;
    *(unsigned long long*) buffers::shared_sink = original_dst;
    flags = 0;
    __asm(
            ".intel_syntax noprefix;"
            "pushf;"
            "push rcx;"
            "popf;"
            "cmp QWORD PTR [rax], rdx;"
            "pushf;"
            "pop rcx;"
            "popf;"
            ".att_syntax;"
            : "+c" (flags)
            : "a" (buffers::shared_sink), "d" (source), "m" (buffers::shared_sink)
    );
    TEST_RESULT("cmp m64, r64 - equal", !(flags & ZF_MASK))
    // 64-bit ----------------------------------------------------------------------------------------------------------


    FINISH_TEST("cmp tests successful!", "cmp tests failed!")
}


void            instruction_tests::test_0x3b                        ()
{

}


void            instruction_tests::test_0x83                        ()
{
    START_TEST("Running tests for grp 1 imm8 (0x83)\n")

    unsigned long long eflags;
    unsigned long long original = 0x8877665544332211;

    // 111 - cmp -------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    __asm(
            ".intel_syntax noprefix;"
            "cmp QWORD PTR [rax], 0x00;"
            "pushf;"
            "pop r8;"
            "mov QWORD PTR [rdx], r8;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&eflags)
    );

    *(unsigned long long*) buffers::shared_sink = original;
    __asm(
            ".intel_syntax noprefix;"
            "cmp QWORD PTR [rax], 0xf1;"
            "pushf;"
            "pop r8;"
            "mov QWORD PTR [rdx], r8;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&eflags)
    );

    *(unsigned long long*) buffers::shared_sink = original;
    __asm(
            ".intel_syntax noprefix;"
            "cmp DWORD PTR [rax], 0x00000000000000e1;"
            "pushf;"
            "pop r8;"
            "mov QWORD PTR [rdx], r8;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&eflags)
    );
    // 111 - cmp -------------------------------------------------------------------------------------------------------

    FINISH_TEST("Grp 1 tests successful!", "Grp 1 tests failed!")
}


void            instruction_tests::test_0x87                        ()
{
    START_TEST("running tests for xchg (0x87)...\n");

    unsigned long long sink_src;
    unsigned long long orig_src = 0x1122334455667788;
    unsigned long long orig_shm = 0x8877665544332211;
    *(__uint64_t*) buffers::shared_sink = orig_shm;

    // 64-bit ----------------------------------------------------------------------------------------------------------
    sink_src = orig_src;
    *(__uint64_t*) buffers::shared_sink = orig_shm;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov r15, QWORD PTR [rdx];"
            "lock xchg QWORD PTR [rax], r15;"
            "mov QWORD PTR [rdx], r15;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&sink_src)
            : "r15"
    );
    TEST_RESULT("xchg m64, r64",
                (__uint64_t) sink_src == (__uint64_t) orig_shm &&
                *(__uint64_t*) buffers::shared_sink == (__uint64_t) orig_src);
    // 64-bit ----------------------------------------------------------------------------------------------------------

    // 16-bit ----------------------------------------------------------------------------------------------------------
    sink_src = orig_src;
    *(__uint64_t*) buffers::shared_sink = orig_shm;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov r15w, WORD PTR [rdx];"
            "lock xchg WORD PTR [rax], r15w;"
            "mov WORD PTR [rdx], r15w;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&sink_src)
            : "r15"
    );
    TEST_RESULT("xchg m16, r16",
                (__uint16_t) sink_src == (__uint16_t) orig_shm &&
                *(__uint16_t*) buffers::shared_sink == (__uint16_t) orig_src &&
                testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE,
                        ((__uint8_t*) &orig_shm) + WORD_SIZE, QWORD_SIZE - DWORD_SIZE) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &sink_src) + WORD_SIZE,
                        ((__uint8_t*) &orig_src) + WORD_SIZE, QWORD_SIZE - DWORD_SIZE) == 0);
    // 16-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    sink_src = orig_src;
    *(__uint64_t*) buffers::shared_sink = orig_shm;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov r15d, DWORD PTR [rdx];"
            "lock xchg DWORD PTR [rax], r15d;"
            "mov DWORD PTR [rdx], r15d;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&sink_src)
            : "r15"
    );
    TEST_RESULT("xchg m32, r32",
                (__uint32_t) sink_src == (__uint32_t) orig_shm &&
                *(__uint32_t*) buffers::shared_sink == (__uint32_t) orig_src &&
                testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE,
                        ((__uint8_t*) &orig_shm) + DWORD_SIZE, QWORD_SIZE - DWORD_SIZE) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &sink_src) + DWORD_SIZE,
                        ((__uint8_t*) &orig_src) + DWORD_SIZE, QWORD_SIZE - DWORD_SIZE) == 0);
    // 32-bit ----------------------------------------------------------------------------------------------------------

    testing_aid::clear_buffer(buffers::shared_sink, 8);

    FINISH_TEST("xchg tests successful!", "xchg tests failed!");
}


void            instruction_tests::test_0x89                        ()
{
    START_TEST("running tests for move (0x89)...\n")

    unsigned long long original = 0x1122334455667788;
    unsigned long long zero     = 0x00;

    // 16-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = 0x00;
    asm
    (
            ".intel_syntax noprefix;"
            "mov WORD PTR [%[dst]], %[src];"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint16_t) original)
            : "memory"
    );
    TEST_RESULT("mov m16, r16", (__uint16_t) original == *(__uint16_t*) buffers::shared_sink &&
            testing_aid::compare_buffers((__uint8_t*) &zero + 2, buffers::shared_sink + 2, DWORD_SIZE) == 0)
    // 16-bit ----------------------------------------------------------------------------------------------------------


    // 32-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = 0x00;
    asm
    (
            ".intel_syntax noprefix;"
            "mov DWORD PTR [%[dst]], %[src];"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint32_t) original)
            : "memory"
    );
    TEST_RESULT("mov m32, r32", (__uint32_t) original == *(__uint32_t*) buffers::shared_sink &&
            *((__uint32_t*) buffers::shared_sink + 1) == 0x00)
    // 32-bit ----------------------------------------------------------------------------------------------------------


    // 64-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = 0x00;
    asm
    (
            ".intel_syntax noprefix;"
            "mov QWORD PTR [%[dst]], %[src];"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" ((__uint64_t) original)
            : "memory"
    );
    TEST_RESULT("mov m64, r64", (__uint64_t) original == *(__uint64_t*) buffers::shared_sink)
    // 64-bit ----------------------------------------------------------------------------------------------------------


    FINISH_TEST("move tests successful!", "move tests failed!")
}


void            instruction_tests::test_0x8b                        ()
{
    START_TEST("running tests for mov (0x8b)...\n");

    unsigned long long original     = 0xf0f0f0f0f0f0f0f0;
    unsigned long long original_dst = 0x0f0f0f0f0f0f0f0f;
    unsigned long long destination  = 0x00;

    // 64-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    destination = original_dst;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdx, QWORD PTR [rax];"
            ".att_syntax;"
            : "+d" (destination)
            : "a" (buffers::shared_sink)
            : "r8"
    );
    TEST_RESULT("mov r64, QWORD PTR", destination == original)
    // 64-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    destination = original_dst;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov edx, DWORD PTR [rax];"
            ".att_syntax;"
            : "+d" (destination)
            : "a" (buffers::shared_sink)
            : "r8"
    );
    logf("%llx\n", destination);
    TEST_RESULT("mov r32, DWORD PTR", *(__uint32_t*)&destination == *(__uint32_t*)&original &&
            *((__uint32_t*)&destination + 1) == 0x00)
    // 32-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    *(unsigned long long*) buffers::shared_sink = original;
    destination = original_dst;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov dx, WORD PTR [rax];"
            ".att_syntax;"
            : "+d" (destination)
            : "a" (buffers::shared_sink)
            : "r8"
    );
    TEST_RESULT("mov r16, WORD PTR", *(__uint16_t*)&destination == *(__uint16_t*)&original &&
            testing_aid::compare_buffers((__uint8_t*) &destination + 3, (__uint8_t*) &original_dst + 3, 3 * WORD_SIZE))
    // 32-bit ----------------------------------------------------------------------------------------------------------

    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    FINISH_TEST("mov (0x8b) tests successful!", "mov (0x8b) tests failed!");
}


void            instruction_tests::test_0xa4                        ()
{
    START_TEST("running tests for movsb (0xa4)...\n");

    // non rep tests ---------------------------------------------------------------------------------------------------
    /*
    unsigned long long offset_1 = 0x01;
    unsigned long long offset_1_sink = 0x07;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping + offset_1), "d" (buffers::shared_sink + offset_1_sink)
            :
    );

    TEST_RESULT("movs WORD PTR [rdi], WORD PTR [rsi]; (shared <- shared | offset 2x)",
            testing_aid::compare_buffers(buffers::shared_mapping + offset_1,
                    buffers::shared_sink + offset_1_sink, BYTE_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink + offset_1_sink, BYTE_SIZE);

    unsigned long long offset_2 = 0x05;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping + offset_2), "d" (buffers::big_buffer_sink)
            :
    );
    TEST_RESULT("movs WORD PTR [rdi], WORD PTR [rsi]; (private <- shared | offset)",
            testing_aid::compare_buffers(buffers::shared_mapping + offset_2, buffers::big_buffer_sink,
                    BYTE_SIZE) == 0)
    testing_aid::clear_buffer(buffers::big_buffer_sink, BYTE_SIZE);

    unsigned long long offset_3 = 0x0a;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::big_buffer), "d" (buffers::shared_sink + offset_3)
            :
    );
    // logf_buff(buffers::big_buffer, SHARED_SIZE);
    // logf_buff(buffers::shared_sink, DQWORD_SIZE);
    TEST_RESULT("movs WORD PTR [rdi], WORD PTR [rsi]; (shared <- private | offset)",
            testing_aid::compare_buffers(buffers::big_buffer, buffers::shared_sink + offset_3,
                    BYTE_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink + offset_3, BYTE_SIZE);
     */
    // non rep tests ---------------------------------------------------------------------------------------------------


    // rep tests -------------------------------------------------------------------------------------------------------
    // rep 2 bytes
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping), "d" (buffers::shared_sink), "c" (WORD_SIZE)
            :
    );
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- 2 | shared <- shared)",
                testing_aid::compare_buffers(buffers::shared_mapping, buffers::shared_sink, WORD_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink, WORD_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping), "d" (buffers::dqword_sink), "c" (WORD_SIZE)
            :
    );
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- 2 | private <- shared)",
                testing_aid::compare_buffers(buffers::shared_mapping, buffers::dqword_sink, WORD_SIZE) == 0)
    testing_aid::clear_buffer(buffers::dqword_sink, WORD_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::dqword), "d" (buffers::shared_sink), "c" (WORD_SIZE)
            :
    );
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- 2 | shared <- private)",
                testing_aid::compare_buffers(buffers::dqword, buffers::shared_sink, WORD_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink, WORD_SIZE);


    // rep 16 bytes
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping), "d" (buffers::shared_sink), "c" (DQWORD_SIZE)
            :
    );
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- 16 | shared <- shared)",
                testing_aid::compare_buffers(buffers::shared_mapping, buffers::shared_sink, DQWORD_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink, DQWORD_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping), "d" (buffers::dqword_sink), "c" (DQWORD_SIZE)
            :
    );
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- 16 | private <- shared)",
                testing_aid::compare_buffers(buffers::shared_mapping, buffers::dqword_sink, DQWORD_SIZE) == 0)
    testing_aid::clear_buffer(buffers::dqword_sink, DQWORD_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::dqword), "d" (buffers::shared_sink), "c" (DQWORD_SIZE)
            :
    );
    // logf_buff(buffers::dqword, DQWORD_SIZE);
    // logf_buff(buffers::shared_sink, DQWORD_SIZE);
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- 16 | shared <- private)",
                testing_aid::compare_buffers(buffers::dqword, buffers::shared_sink, DQWORD_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink, DQWORD_SIZE);

    // rep max bytes
    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping), "d" (buffers::shared_sink), "c" (SHARED_SIZE)
            :
    );

    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- SHARED_SIZE | shared <- shared)",
                testing_aid::compare_buffers(buffers::shared_mapping, buffers::shared_sink, SHARED_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_mapping), "d" (buffers::big_buffer_sink), "c" (SHARED_SIZE)
            :
    );
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- SHARED_SIZE | private <- shared)",
            testing_aid::compare_buffers(buffers::shared_mapping, buffers::big_buffer_sink, SHARED_SIZE) == 0)
    testing_aid::clear_buffer(buffers::big_buffer_sink, SHARED_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "mov rdi, rdx;"
            "mov rsi, rax;"
            "rep movsb;"
            ".att_syntax;"
            :
            : "a" (buffers::big_buffer), "d" (buffers::shared_sink), "c" (SHARED_SIZE)
            :
    );
    // logf_buff(buffers::big_buffer, SHARED_SIZE);
    // logf_buff(buffers::shared_sink, DQWORD_SIZE);
    TEST_RESULT("rep movs WORD PTR [rdi], WORD PTR [rsi]; (rcx <- SHARED_SIZE | shared <- private)",
                testing_aid::compare_buffers(buffers::big_buffer, buffers::shared_sink, SHARED_SIZE) == 0)
    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    // rep tests -------------------------------------------------------------------------------------------------------

    // success or failure
    FINISH_TEST("movsb tests successful!", "movsb tests failed!")
}


void            instruction_tests::test_0xab                        ()
{
    START_TEST("Running tests for stos (0xab)\n")
    unsigned long long src = 0x1122334455667788;
    void* passed_dst;

    // 16-bit ----------------------------------------------------------------------------------------------------------
    passed_dst = buffers::shared_sink;
    __asm
    (
            ".intel_syntax noprefix;"
            "std;"
            "stosw;"
            ".att_syntax;"
            : "+D" (passed_dst)
            : "a" (src)
            : "memory"
    );
    logf("(__uint16_t) src == *(__uint16_t*) buffers::shared_sink: %s | %llx == %llx (%llx)\n",
            (__uint16_t) src == *(__uint16_t*) buffers::shared_sink ? "good" : "fail",
            (__uint16_t) src, *(__uint16_t*) buffers::shared_sink, *((__uint16_t*) passed_dst + 1));
    TEST_RESULT("stosw | df set",
            (__uint16_t) src == *(__uint16_t*) buffers::shared_sink &&
            (__uint32_t) src != *(__uint32_t*) buffers::shared_sink &&
            passed_dst == (__uint16_t*) buffers::shared_sink - 1)
    *(__uint64_t*) buffers::shared_sink = 0x00;

    passed_dst = buffers::shared_sink;
    __asm
    (
            ".intel_syntax noprefix;"
            "cld;"
            "stosw;"
            ".att_syntax;"
            : "+D" (passed_dst)
            : "a" (src)
            : "memory"
    );
    TEST_RESULT("stosw | df cleared",
            (__uint16_t) src == *(__uint16_t*) buffers::shared_sink &&
            (__uint32_t) src != *(__uint32_t*) buffers::shared_sink &&
            passed_dst == (__uint16_t*) buffers::shared_sink + 1)
    *(__uint64_t*) buffers::shared_sink = 0x00;
    // 16-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    passed_dst = buffers::shared_sink;
    __asm
    (
            ".intel_syntax noprefix;"
            "std;"
            "stosd;"
            ".att_syntax;"
            : "+D" (passed_dst)
            : "a" (src)
            : "memory"
    );
    TEST_RESULT("stosd | df set",
                (__uint32_t) src == *(__uint32_t*) buffers::shared_sink &&
                *((__uint32_t*)&src + 1) != *((__uint32_t*) buffers::shared_sink + 1) &&
                passed_dst == (__uint32_t*) buffers::shared_sink - 1)
    *(__uint64_t*) buffers::shared_sink = 0x00;

    passed_dst = buffers::shared_sink;
    __asm
    (
            ".intel_syntax noprefix;"
            "cld;"
            "stosd;"
            ".att_syntax;"
            : "+D" (passed_dst)
            : "a" (src)
            : "memory"
    );
    TEST_RESULT("stosd | df cleared",
                (__uint32_t) src == *(__uint32_t*) buffers::shared_sink &&
                *((__uint32_t*)&src + 1) != *((__uint32_t*) buffers::shared_sink + 1) &&
                passed_dst == (__uint32_t*) buffers::shared_sink + 1)
    *(__uint64_t*) buffers::shared_sink = 0x00;
    // 32-bit ----------------------------------------------------------------------------------------------------------

    // 64-bit ----------------------------------------------------------------------------------------------------------
    passed_dst = buffers::shared_sink;
    __asm
    (
            ".intel_syntax noprefix;"
            "std;"
            "stosq;"
            ".att_syntax;"
            : "+D" (passed_dst)
            : "a" (src)
            : "memory"
    );
    TEST_RESULT("stosq | df set",
                (__uint64_t) src == *(__uint64_t*) buffers::shared_sink &&
                passed_dst == (__uint64_t*) buffers::shared_sink - 1)
    *(__uint64_t*) buffers::shared_sink = 0x00;

    passed_dst = buffers::shared_sink;
    __asm
    (
            ".intel_syntax noprefix;"
            "cld;"
            "stosq;"
            ".att_syntax;"
            : "+D" (passed_dst)
            : "a" (src)
            : "memory"
    );
    TEST_RESULT("stosq | df cleared",
                (__uint64_t) src == *(__uint64_t*) buffers::shared_sink &&
                passed_dst == (__uint64_t*) buffers::shared_sink + 1)
    *(__uint64_t*) buffers::shared_sink = 0x00;
    // 64-bit ----------------------------------------------------------------------------------------------------------

    FINISH_TEST("stos tests successful!", "stos tests failed!")
}


void            instruction_tests::test_0xc7                        ()
{
    START_TEST("Running tests for Grp 11 - mov m ,imm (0xc7) \n")


    unsigned long long zero = 0x00;
    unsigned long long sign = -1;
    unsigned long long src  = 0x8877665544332211;


    // 16-bit ----------------------------------------------------------------------------------------------------------
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov WORD PTR [rax], 0x2211;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m16, imm16 | non-zero",
                *(__uint16_t*) buffers::shared_sink == (__uint16_t) src &&
                testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, ((__uint8_t*) &zero) + WORD_SIZE,
                        QWORD_SIZE - WORD_SIZE) == 0)


    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &src), QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov WORD PTR [rax], 0x0000;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m16, imm16 | zero",
                *(__uint16_t*) buffers::shared_sink == (__uint16_t) zero &&
                testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, ((__uint8_t*) &src) + WORD_SIZE,
                        QWORD_SIZE - WORD_SIZE) == 0)
    // 16-bit ----------------------------------------------------------------------------------------------------------


    // 32-bit ----------------------------------------------------------------------------------------------------------
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov DWORD PTR [rax], 0x44332211;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m32, imm32 | non-zero",
                *(__uint32_t*) buffers::shared_sink == (__uint32_t) src &&
                testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, ((__uint8_t*) &zero) + DWORD_SIZE,
                        QWORD_SIZE - DWORD_SIZE) == 0)


    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &src), QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov DWORD PTR [rax], 0x00000000;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m32, imm32 | zero",
                *(__uint32_t*) buffers::shared_sink == (__uint32_t) zero &&
                testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, ((__uint8_t*) &src) + DWORD_SIZE,
                        QWORD_SIZE - DWORD_SIZE) == 0)
    // 32-bit ----------------------------------------------------------------------------------------------------------


    // 64-bit ----------------------------------------------------------------------------------------------------------
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov QWORD PTR [rax], 0x44332211;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m64, imm32 | non-zero",
                *(__uint32_t*) buffers::shared_sink == (__uint32_t) src &&
                testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, ((__uint8_t*) &zero) + DWORD_SIZE,
                        QWORD_SIZE - DWORD_SIZE) == 0)


    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &src), QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov QWORD PTR [rax], 0x00000000;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m64, imm32 | zero",
                *(__uint32_t*) buffers::shared_sink == (__uint32_t) zero &&
                testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, ((__uint8_t*) &zero) + DWORD_SIZE,
                        QWORD_SIZE - DWORD_SIZE) == 0)


    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &src), QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "mov QWORD PTR [rax], 0xfffffffff0000000;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink)
    );
    TEST_RESULT("mov m64, imm32 | sign extend",
                *(__uint32_t*) buffers::shared_sink == (__uint32_t) 0xf0000000 &&
                testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, ((__uint8_t*) &sign) + DWORD_SIZE,
                        QWORD_SIZE - DWORD_SIZE) == 0)
    // 64-bit ----------------------------------------------------------------------------------------------------------


    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    FINISH_TEST("Grp 11 - mov m ,imm tests successful!", "Grp 11 - mov m ,imm tests failed!")
}


void            instruction_tests::test_0x0f_0x11                   ()
{
    START_TEST("Running tests for movups (0x0f 0x11)\n")


    unsigned long long original[2] = { 0x1122334455667788, 0x8877665544332211 };
    asm
    (
            ".intel_syntax noprefix;"
            "movups xmm0, XMMWORD PTR [%[src]];"
            "movups XMMWORD PTR [%[dst]], xmm0;"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
    );
    TEST_RESULT("movups m128, xmm", testing_aid::compare_buffers(buffers::shared_sink,
            (__uint8_t*) original, DQWORD_SIZE) == 0)

    testing_aid::clear_buffer(buffers::shared_sink, DQWORD_SIZE);
    FINISH_TEST("movups tests successful!", "movups tests failed!")
}


void            instruction_tests::test_0x0f_0xb1                   ()
{
    START_TEST("Running tests for cmpxchg (0x0f 0xb1)\n")

    __uint8_t orig_rcx[]  = QWORD_CONTENT_REVERSE;
    __uint8_t orig_rax[]  = QWORD_CONTENT;

    __uint8_t rax[]       = QWORD_CONTENT;
    __uint8_t rcx[]       = QWORD_CONTENT_REVERSE;

    __uint8_t orig_buf[8] = CONTENT_EMPTY;

    // 64-bit ==========================================================================================================
    //
    // qword dst == rax
    //  => qword dst == src
    //  => qword dst != rax
    //  => orig rax  == rax
    //  => orig src  == src
    //

    COPY_BUFFERS(buffers::shared_sink, rax, QWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "lock cmpxchg QWORD PTR [rdx], rcx;"
            "mov QWORD PTR [rbx], rcx;"
            ".att_syntax;"
            :
            : "a" (*(unsigned long long*) rax), "d" (buffers::shared_sink), "c" (*(unsigned long long*) rcx),
                    "b" (rcx)
    );
    TEST_RESULT("cmpxch QWORD PTR[rdi], rcx; | rax == qword dst",
            testing_aid::compare_buffers(buffers::shared_sink, orig_rcx, QWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0)
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, orig_rcx, QWORD_SIZE) == 0 ?
    //                       "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) != 0 ?
    //                       "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) == 0 ? "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0 ? "good" : "bad");
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);


    //
    // qword dst != rax
    //  => qword dst != src
    //  => qword dst == rax
    //  => orig rax  != rax
    //  => orig src  == src
    //

    __asm(
            ".intel_syntax noprefix;"
            "lock cmpxchg QWORD PTR [rdx], rcx;"
            "mov QWORD PTR [rbx], rcx;"
            "mov QWORD PTR [rsi], rax;"
            ".att_syntax;"
            :
            : "a" (*(unsigned long long*) rax), "d" (buffers::shared_sink), "c" (*(unsigned long long*) rcx),
                    "b" (rcx), "S" (rax)
    );
    TEST_RESULT("cmpxch QWORD PTR[rdi], rcx; | rax != qword dst",
            testing_aid::compare_buffers(buffers::shared_sink, rcx, QWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0)
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rcx, QWORD_SIZE) != 0 ?
    //         "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) == 0 ?
    //         "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) != 0 ? "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0 ? "good" : "bad");
    COPY_BUFFERS(rax, orig_rax, QWORD_SIZE);
    // 64-bit ==========================================================================================================


    // 32-bit ==========================================================================================================
    //
    // dword dst == rax
    //  =>        dword dst == src
    //  =>        dword dst != eax
    //  => higher dword dst != higher eax
    //  => higher dword dst != higher src
    //  => higher dword dst == empty
    //  =>        orig eax  == eax
    //  =>        orig src  == src
    //

    COPY_BUFFERS(buffers::shared_sink, rax, DWORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "lock cmpxchg DWORD PTR [rdx], ecx;"
            "mov DWORD PTR [rbx], ecx;"
            "mov DWORD PTR [rsi], eax;"
            ".att_syntax;"
            :
            : "a" (*(unsigned long long*) rax), "d" (buffers::shared_sink), "c" (*(unsigned long long*) rcx),
                    "b" (rcx), "S" (rax)
    );
    TEST_RESULT("cmpxch DWORD PTR[rdi], ecx; | eax == dword dst",
            testing_aid::compare_buffers(buffers::shared_sink, orig_rcx, DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink, rax, DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, orig_rax + DWORD_SIZE,
                    QWORD_SIZE - DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, orig_rcx + DWORD_SIZE,
                    QWORD_SIZE - DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, orig_buf + DWORD_SIZE,
                    QWORD_SIZE - DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(orig_rax, rax, DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(rcx, orig_rcx, DWORD_SIZE) == 0)
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, orig_rcx, QWORD_SIZE) == 0 ?
    //                       "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) != 0 ?
    //                       "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) == 0 ? "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0 ? "good" : "bad");
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);


    //
    // dword dst != rax
    //  =>        dword dst != src
    //  =>        dword dst == rax
    //  => higher dword dst != higher eax
    //  => higher dword dst != higher src
    //  => higher dword dst == empty
    //  =>        orig rax  != rax
    //  =>        orig src  == src
    //

    __asm(
            ".intel_syntax noprefix;"
            "lock cmpxchg DWORD PTR [rdx], ecx;"
            "mov DWORD PTR [rbx], ecx;"
            "mov DWORD PTR [rsi], eax;"
            ".att_syntax;"
            :
            : "a" (*(unsigned long long*) rax), "d" (buffers::shared_sink), "c" (*(unsigned long long*) rcx),
                    "b" (rcx), "S" (rax)
    );
    TEST_RESULT("cmpxch DWORD PTR[rdi], ecx; | eax != dword dst",
            testing_aid::compare_buffers(buffers::shared_sink, rcx, DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink, rax, DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, orig_rax + DWORD_SIZE,
                                         QWORD_SIZE - DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, orig_rcx + DWORD_SIZE,
                                         QWORD_SIZE - DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + DWORD_SIZE, orig_buf + DWORD_SIZE,
                                         QWORD_SIZE - DWORD_SIZE) == 0 &&
            testing_aid::compare_buffers(orig_rax, rax, DWORD_SIZE) != 0 &&
            testing_aid::compare_buffers(rcx, orig_rcx, DWORD_SIZE) == 0)
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rcx, QWORD_SIZE) != 0 ?
    //         "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) == 0 ?
    //         "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) != 0 ? "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0 ? "good" : "bad");
    COPY_BUFFERS(rax, orig_rax, QWORD_SIZE);
    // 32-bit ==========================================================================================================


    // 16-bit ==========================================================================================================
    //
    // dword dst == rax
    //  =>   word dst == src
    //  =>   word dst != ax
    //  => higher dst != higher ax
    //  => higher dst != higher src
    //  => higher dst == empty
    //  =>   orig eax == ax
    //  =>   orig src == src
    //

    COPY_BUFFERS(buffers::shared_sink, rax, WORD_SIZE);
    __asm(
            ".intel_syntax noprefix;"
            "lock cmpxchg WORD PTR [rdx], cx;"
            "mov WORD PTR [rbx], cx;"
            "mov WORD PTR [rsi], ax;"
            ".att_syntax;"
            :
            : "a" (*(unsigned long long*) rax), "d" (buffers::shared_sink), "c" (*(unsigned long long*) rcx),
                    "b" (rcx), "S" (rax)
    );
    TEST_RESULT("cmpxch WORD PTR[rdi], cx; | ax == word dst",
            testing_aid::compare_buffers(buffers::shared_sink, orig_rcx, WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink, rax, WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, orig_rax + WORD_SIZE,
                                         QWORD_SIZE - WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, orig_rcx + WORD_SIZE,
                                         QWORD_SIZE - WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, orig_buf + WORD_SIZE,
                                         QWORD_SIZE - WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(orig_rax, rax, WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(rcx, orig_rcx, WORD_SIZE) == 0)
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) != 0 ?
    //                       "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) == 0 ? "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0 ? "good" : "bad");
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);


    //
    // qword dst != rax
    //  => qword dst != src
    //  => qword dst == ax
    //  => orig ax   != ax
    //  => orig src  == src
    //

    __asm(
            ".intel_syntax noprefix;"
            "lock cmpxchg WORD PTR [rdx], cx;"
            "mov WORD PTR [rbx], cx;"
            "mov WORD PTR [rsi], ax;"
            ".att_syntax;"
            :
            : "a" (*(unsigned long long*) rax), "d" (buffers::shared_sink), "c" (*(unsigned long long*) rcx),
                    "b" (rcx), "S" (rax)
    );
    TEST_RESULT("cmpxch WORD PTR[rdi], cx; | ax != word dst",
            testing_aid::compare_buffers(buffers::shared_sink, rcx, WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink, rax, WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, orig_rax + WORD_SIZE,
                                         QWORD_SIZE - WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, orig_rcx + WORD_SIZE,
                                         QWORD_SIZE - WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(buffers::shared_sink + WORD_SIZE, orig_buf + WORD_SIZE,
                                         QWORD_SIZE - WORD_SIZE) == 0 &&
            testing_aid::compare_buffers(orig_rax, rax, WORD_SIZE) != 0 &&
            testing_aid::compare_buffers(rcx, orig_rcx, WORD_SIZE) == 0)
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rcx, QWORD_SIZE) != 0 ?
    //         "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(buffers::shared_sink, rax, QWORD_SIZE) == 0 ?
    //         "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(orig_rax, rax, QWORD_SIZE) != 0 ? "good" : "bad");
    // printf("first: %s\n", testing_aid::compare_buffers(rcx, orig_rcx, QWORD_SIZE) == 0 ? "good" : "bad");
    COPY_BUFFERS(rax, orig_rax, QWORD_SIZE);
    // 16-bit ==========================================================================================================


    FINISH_TEST("cmpxchg tests successful!", "cmpxchg tests failed!")
}


void            instruction_tests::test_0x0f_0x6f                   ()
{
    START_TEST("Running tests for 0x0f 0x6f...\n")

    // movq ------------------------------------------------------------------------------------------------------------
    unsigned long long destination[] = { 0xffffffffffffffff, 0xffffffffffffffff };
    *(unsigned long long*) buffers::shared_sink = 0x1122334455667788;
    __asm__
    (
            ".intel_syntax noprefix;"
            "movq mm0, QWORD PTR [%[src]];"
            "movq QWORD PTR [%[dst]], mm0;"
            ".att_syntax;"
            :
            : "m" (buffers::shared_sink), "m" (destination), [dst] "r" (destination), [src] "r" (buffers::shared_sink)
            : "mm0"
    );
    // movq ------------------------------------------------------------------------------------------------------------

    FINISH_TEST("0x0f 0x6f tests successful!", "0x0f 0x6f tests failed!")
}


void            instruction_tests::test_0x0f_0x7f                   ()
{
    START_TEST("Running tests for 0x0f 0x7f...\n")

    unsigned long long original[] = { 0x1122334455667788, 0x8877665544332211 };
    testing_aid::clear_buffer(buffers::shared_sink, 2 * QWORD_SIZE);

    // movq ------------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movq mm0, QWORD PTR [%[src]];"
            "movq QWORD PTR [%[dst]], mm0;"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
            : "memory", "mm0"
    );
    TEST_RESULT("movq m64, mm",
            testing_aid::compare_buffers((__uint8_t*) original, buffers::shared_sink, QWORD_SIZE) == 0);
    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    // movq ------------------------------------------------------------------------------------------------------------

    // movdqa ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movdqa xmm0, XMMWORD PTR [%[src]];"
            "movdqa XMMWORD PTR [%[dst]], xmm0;"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
            : "memory", "xmm0"
    );
    TEST_RESULT("movdqa m128, xmm",
            testing_aid::compare_buffers((__uint8_t*) original, buffers::shared_sink, 2 + QWORD_SIZE) == 0);
    testing_aid::clear_buffer(buffers::shared_sink, 2 * QWORD_SIZE);
    // movdqa ----------------------------------------------------------------------------------------------------------

    // movdqu ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movdqu xmm0, XMMWORD PTR [%[src]];"
            "movdqu XMMWORD PTR [%[dst]], xmm0;"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
            : "memory", "xmm0"
    );
    TEST_RESULT("movdqu m128, xmm",
            testing_aid::compare_buffers((__uint8_t*) original, buffers::shared_sink, 2 + QWORD_SIZE) == 0);
    testing_aid::clear_buffer(buffers::shared_sink, 2 * QWORD_SIZE);
    // movdqu ----------------------------------------------------------------------------------------------------------



    FINISH_TEST("0x0f 0x7f tests successful!", "0x0f 0x7f tests failed!")
}


void            instruction_tests::test_0x0f_0xbe                   ()
{
    START_TEST("running tests for movsx (0x0f 0xbe)\n")

    unsigned long long dst          = 0x00;
    unsigned long long src_orig_neg = 0b10001111;
    unsigned long long src_orig_pos = 0b01001111;
    unsigned long long sign         = 0x00ffffffffffffff;
    unsigned long long zero         = 0x00;

    COPY_BUFFERS(buffers::shared_sink, (__uint8_t*) &src_orig_pos, 1);
    // 16-bit ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movsx r15w, BYTE PTR [rdx];"
            "mov WORD PTR [rax], r15w;"
            ".att_syntax;"
            :
            : "a" (&dst), "d" (buffers::shared_sink)
            : "r15"
    );
    TEST_RESULT("movsx 16-bit | positive source",
            testing_aid::compare_buffers((__uint8_t*) &dst, (__uint8_t*) &src_orig_pos, 1) == 0 &&
            testing_aid::compare_buffers(((__uint8_t*) &dst) + 1, (__uint8_t*) &zero, 1) == 0)
    dst = 0x00;
    // 16-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movsx r15d, BYTE PTR [rdx];"
            "mov DWORD PTR [rax], r15d;"
            ".att_syntax;"
            :
            : "a" (&dst), "d" (buffers::shared_sink)
            : "r15"
    );
    TEST_RESULT("movsx 32-bit | positive source",
                testing_aid::compare_buffers((__uint8_t*) &dst, (__uint8_t*) &src_orig_pos, 1) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &dst) + 1, (__uint8_t*) &zero, 3) == 0)
    dst = 0x00;
    // 32-bit ----------------------------------------------------------------------------------------------------------

    // 64-bit ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movsx r15, BYTE PTR [rdx];"
            "mov QWORD PTR [rax], r15;"
            ".att_syntax;"
            :
            : "a" (&dst), "d" (buffers::shared_sink)
            : "r15"
    );
    TEST_RESULT("movsx 64-bit | positive source",
                testing_aid::compare_buffers((__uint8_t*) &dst, (__uint8_t*) &src_orig_pos, 1) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &dst) + 1, (__uint8_t*) &zero, 7) == 0)
    dst = 0x00;
    // 64-bit ----------------------------------------------------------------------------------------------------------

    COPY_BUFFERS(buffers::shared_sink, (__uint8_t*) &src_orig_neg, 1);
    // 16-bit ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movsx r15w, BYTE PTR [rdx];"
            "mov WORD PTR [rax], r15w;"
            ".att_syntax;"
            :
            : "a" (&dst), "d" (buffers::shared_sink)
            : "r15"
    );
    TEST_RESULT("movsx 16-bit | negative source",
                testing_aid::compare_buffers((__uint8_t*) &dst, (__uint8_t*) &src_orig_neg, 1) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &dst) + 1, (__uint8_t*) &sign, 1) == 0)
    dst = 0x00;
    // 16-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movsx r15d, BYTE PTR [rdx];"
            "mov DWORD PTR [rax], r15d;"
            ".att_syntax;"
            :
            : "a" (&dst), "d" (buffers::shared_sink)
            : "r15"
    );
    TEST_RESULT("movsx 32-bit | negative source",
                testing_aid::compare_buffers((__uint8_t*) &dst, (__uint8_t*) &src_orig_neg, 1) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &dst) + 1, (__uint8_t*) &sign, 3) == 0)
    dst = 0x00;
    // 32-bit ----------------------------------------------------------------------------------------------------------

    // 64-bit ----------------------------------------------------------------------------------------------------------
    __asm
    (
            ".intel_syntax noprefix;"
            "movsx r15, BYTE PTR [rdx];"
            "mov QWORD PTR [rax], r15;"
            ".att_syntax;"
            :
            : "a" (&dst), "d" (buffers::shared_sink)
            : "r15"
    );
    TEST_RESULT("movsx 64-bit | negative source",
                testing_aid::compare_buffers((__uint8_t*) &dst, (__uint8_t*) &src_orig_neg, 1) == 0 &&
                testing_aid::compare_buffers(((__uint8_t*) &dst) + 1, (__uint8_t*) &sign, 7) == 0)
    dst = 0x00;
    // 64-bit ----------------------------------------------------------------------------------------------------------

    testing_aid::clear_buffer(buffers::shared_sink, 8);
    FINISH_TEST("movsx tests successful!", "movsx tests failed!")
}


void            instruction_tests::test_0x0f_0xc1                   ()
{
    START_TEST("running tests for xadd (0x0f 0xc1)\n")

    unsigned long long source;
    unsigned long long orig_src = 0x1122112211221122;
    unsigned long long original = 0x2211221122112211;
    unsigned long long test     = orig_src + original;

    // 16-bit ----------------------------------------------------------------------------------------------------------
    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &original), WORD_SIZE);
    source = orig_src;
    __asm (
            ".intel_syntax noprefix;"
            "mov r8w, WORD PTR [rdx];"
            "xadd WORD PTR [rax], r8w;"
            "mov WORD PTR [rdx], r8w;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&source)
            : "r8"
    );
    TEST_RESULT("xadd WORD PTR [reg], regw",
            testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &test, WORD_SIZE) == 0 &&
            testing_aid::compare_buffers((__uint8_t*) &source, (__uint8_t*) &original, WORD_SIZE) == 0)
    // 16-bit ----------------------------------------------------------------------------------------------------------

    // 32-bit ----------------------------------------------------------------------------------------------------------
    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &original), DWORD_SIZE);
    source = orig_src;
    __asm (
            ".intel_syntax noprefix;"
            "mov r8d, DWORD PTR [rdx];"
            "xadd DWORD PTR [rax], r8d;"
            "mov DWORD PTR [rdx], r8d;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&source)
            : "r8"
    );
    TEST_RESULT("xadd WORD PTR [reg], regw",
                testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &test, DWORD_SIZE) == 0 &&
                testing_aid::compare_buffers((__uint8_t*) &source, (__uint8_t*) &original, DWORD_SIZE) == 0)
    // 32-bit ----------------------------------------------------------------------------------------------------------

    // 64-bit ----------------------------------------------------------------------------------------------------------
    COPY_BUFFERS(buffers::shared_sink, ((__uint8_t*) &original), QWORD_SIZE);
    source = orig_src;
    __asm (
            ".intel_syntax noprefix;"
            "mov r8, QWORD PTR [rdx];"
            "xadd QWORD PTR [rax], r8;"
            "mov QWORD PTR [rdx], r8;"
            ".att_syntax;"
            :
            : "a" (buffers::shared_sink), "d" (&source)
            : "r8"
    );
    TEST_RESULT("xadd WORD PTR [reg], regw",
                testing_aid::compare_buffers(buffers::shared_sink, (__uint8_t*) &test, QWORD_SIZE) == 0 &&
                testing_aid::compare_buffers((__uint8_t*) &source, (__uint8_t*) &original, QWORD_SIZE) == 0)
    // 64-bit ----------------------------------------------------------------------------------------------------------

    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);

    FINISH_TEST("xadd tests succesful!", "xadd tests failed!")
}


void            instruction_tests::test_0x0f_0xe7                   ()
{
    START_TEST("Running tests for 0x0f 0xe7...\n")

    // movntdq ---------------------------------------------------------------------------------------------------------
    unsigned long long original[] = { 0x1122334455667788, 0x8877665544332211 };
    testing_aid::clear_buffer(buffers::shared_sink, 2 * QWORD_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "movdqu xmm0, XMMWORD PTR [%[src]];"
            "movntdq XMMWORD PTR [%[dst]], xmm0;"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
            : "xmm0", "memory"
    );
    TEST_RESULT("movntdq m128, xmm",
            testing_aid::compare_buffers((__uint8_t*) original, buffers::shared_sink, 2 * QWORD_SIZE) == 0)
    // movntdq ---------------------------------------------------------------------------------------------------------

    // movntdq ---------------------------------------------------------------------------------------------------------
    testing_aid::clear_buffer(buffers::shared_sink, 2 * QWORD_SIZE);

    __asm
    (
            ".intel_syntax noprefix;"
            "movq mm0, QWORD PTR [%[src]];"
            "movntq QWORD PTR [%[dst]], mm0;"
            ".att_syntax;"
            :
            : [dst] "r" (buffers::shared_sink), [src] "r" (original)
            : "mm0", "memory"
    );
    TEST_RESULT("movntq m64, mm",
            testing_aid::compare_buffers((__uint8_t*) original, buffers::shared_sink, QWORD_SIZE) == 0)
    // movntdq ---------------------------------------------------------------------------------------------------------

    testing_aid::clear_buffer(buffers::shared_sink, QWORD_SIZE);
    FINISH_TEST("0x0f 0xe7 tests successful!", "0x0f 0xe7 tests failed!")
}


void            instruction_tests::test_extras                      ()
{
    START_TEST("extra tests\n");

    unsigned long long original = 0xa5a5a5a5a5a5a5a5;
    unsigned long long flags;

    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x58) = original;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov    QWORD PTR [rbx+0x58],0x0;"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink)
    );
    TEST_RESULT("mov    QWORD PTR [rbx+0x58],0x0",
                *(unsigned long long*)(buffers::shared_sink + 0x58) == 0x00)


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x8b) = original;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov    QWORD PTR [rbx+0xb8],0x0;"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink)
    );
    TEST_RESULT("mov    QWORD PTR [rbx+0xb8],0x0",
                *(unsigned long long*)(buffers::shared_sink + 0xb8) == 0x00)


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x58) = original;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "cmp    DWORD PTR [rbx+0x58],0x0;"
            "pushfq;"
            "pop QWORD PTR [rax];"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink), "a" (&flags)
    );
    TEST_RESULT("cmp    DWORD PTR [rbx+0x58],0x0 - fail",
                !(flags & (0x1u << 6u)))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "cmp    DWORD PTR [rbx+0x58],0x0;"
            "pushfq;"
            "pop QWORD PTR [rax];"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink), "a" (&flags)
    );
    TEST_RESULT("cmp    DWORD PTR [rbx+0x58],0x0 - success",
                (flags & (0x1u << 6u)))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x5c) = original;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov    DWORD PTR [rbx+0x5c],0x1;"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink)
    );
    TEST_RESULT("mov    DWORD PTR [rbx+0x5c],0x1",
                *(__uint32_t*)(buffers::shared_sink + 0x5c) == 0x01 &&
                *(__uint32_t*)(buffers::shared_sink + 0x5c + 4) == *((__uint32_t*)&original + 1))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x58) = original;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov    DWORD PTR [rbx+0x58],0x1;"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink)
    );
    TEST_RESULT("mov    DWORD PTR [rbx+0x58],0x1",
                *(__uint32_t*)(buffers::shared_sink + 0x58) == 0x01 &&
                *(__uint32_t*)(buffers::shared_sink + 0x58 + 4) == *((__uint32_t*)&original + 1))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x5c) = original;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "cmp    DWORD PTR [rbx+0x5c],0x0;"
            "pushfq;"
            "pop QWORD PTR [rax];"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink), "a" (&flags)
    );
    TEST_RESULT("cmp    DWORD PTR [rbx+0x5c],0x0 - fail",
                !(flags & (0x1u << 6u)))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "cmp    DWORD PTR [rbx+0x5c],0x0;"
            "pushfq;"
            "pop QWORD PTR [rax];"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink), "a" (&flags)
    );
    TEST_RESULT("cmp    DWORD PTR [rbx+0x5c],0x0 - success",
                (flags & (0x1u << 6u)))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0x5c) = original;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov    DWORD PTR [rbx+0x5c],0x0;"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink)
    );
    TEST_RESULT("mov    DWORD PTR [rbx+0x58],0x1",
                *(__uint32_t*)(buffers::shared_sink + 0x5c) == 0x00 &&
                *(__uint32_t*)(buffers::shared_sink + 0x5c + 4) == *((__uint32_t*)&original + 1))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0xb8) = original;
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "cmp    DWORD PTR [rbx+0xb8],0x0;"
            "pushfq;"
            "pop QWORD PTR [rax];"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink), "a" (&flags)
    );
    TEST_RESULT("cmp    DWORD PTR [rbx+0xb8],0x0 - fail",
                !(flags & (0x1u << 6u)))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    flags = 0;
    __asm
    (
            ".intel_syntax noprefix;"
            "cmp    DWORD PTR [rbx+0xb8],0x0;"
            "pushfq;"
            "pop QWORD PTR [rax];"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink), "a" (&flags)
    );
    TEST_RESULT("cmp    DWORD PTR [rbx+0xb8],0x0 - success",
                (flags & (0x1u << 6u)))


    testing_aid::clear_buffer(buffers::shared_sink, SHARED_SIZE);
    *(unsigned long long*)(buffers::shared_sink + 0xbc) = original;
    __asm
    (
            ".intel_syntax noprefix;"
            "mov    DWORD PTR [rbx+0xbc],0x1;"
            ".att_syntax;"
            :
            : "b" (buffers::shared_sink)
    );
    TEST_RESULT("mov    DWORD PTR [rbx+0xbc],0x1",
                *(__uint32_t*)(buffers::shared_sink + 0xbc) == 0x01 &&
                *(__uint32_t*)(buffers::shared_sink + 0xbc + 4) == *((__uint32_t*)&original + 1))

    FINISH_TEST("extra tests successful\n", "extra tests failed\n");
}
