#include <stdlib.h>
#include <stdio.h>
#define SEA_LIB_IMPLEMENTATION
#include "SEA_lib.h"

int main()
{
    SEA_Result(SEA_VoidPtr) allocator_result = SEA_LinearAllocator_new(2);

    if (allocator_result.err.error_code != 0)
    {
        SEA_Error_print(allocator_result.err);
        return -1;
    }

    SEA_LinearAllocator* linear_allocator = (SEA_LinearAllocator*) allocator_result.value;

    printf("Successfully allocated\n");

    SEA_Result(SEA_VoidPtr) result = SEA_LinearAllocator_alloc_resize_allocator_if_needed(linear_allocator, 4);

    if (result.err.error_code != 0)
    {
        SEA_Error_print(result.err);
        SEA_LinearAllocator_free(linear_allocator);
        return -1;
    }

    return 0;
}