#include <chrono>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define GETPID_TEST_COUNT    10000000
#define LOOP_TIMES	      10

int main()
{
    // getpid benchmark setup ------------------------------------------------------------------------------------------
    for (int size_i = 0; size_i < LOOP_TIMES; size_i++)
    {        
        auto start = std::chrono::high_resolution_clock::now();
        for (int cnt_i = 0; cnt_i < GETPID_TEST_COUNT; cnt_i++)
            getpid();
        auto end = std::chrono::high_resolution_clock::now();
        float result = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        printf("\t> %u: %f ns\n", size_i, result / GETPID_TEST_COUNT);
    }
    return 0;
}
