#include <stdlib.h>
#include <stdio.h>
#define SEA_LIB_IMPLEMENTATION
#include "SEA_lib.h"

int main()
{
    SEA_Result(SEA_VoidPtr) allocator_result = SEA_LinearAllocator_new(50);

    if (allocator_result.err.error_code != 0)
    {
        SEA_Error_print(allocator_result.err);
        return -1;
    }

    SEA_LinearAllocator* linear_allocator = (SEA_LinearAllocator*) allocator_result.value;

    printf("Successfully allocated");

    SEA_LinearAllocator_free(linear_allocator);

    return 0;
}