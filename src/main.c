#include <stdlib.h>
#include <stdio.h>
#define SEA_LIB_IMPLEMENTATION
#include "SEA_lib.h"

SEA_Result_declr(int);
SEA_Vec_declr(int);
SEA_Vec_impl(int);

SEA_Result(int) do_stuff()
{
    SEA_Result(SEA_LinearAllocator) result = SEA_LinearAllocator_new(5);
    SEA_LinearAllocator allocator2 = SEA_try(int, result);
    SEA_LinearAllocator allocator = (SEA_LinearAllocator) { 0 };
    SEA_Result(SEA_Vec(int)) vec_result = SEA_Vec_new(int, &allocator);
    SEA_Vec(int) vec = SEA_try(int, vec_result);
    SEA_Vec_push_back(int, &vec, 4);
    SEA_try_func(allocator, int, SEA_LinearAllocator_new(5));
    SEA_LinearAllocator_free(&allocator2);
    SEA_LinearAllocator_free(&allocator);
    SEA_Vec_free(int, &vec);
}

int main()
{
    SEA_Result(SEA_LinearAllocator) allocator_result = SEA_LinearAllocator_new(2);

    if (allocator_result.err != NULL)
    {
        SEA_ErrRef_print(allocator_result.err);
        return -1;
    }

    SEA_LinearAllocator linear_allocator = allocator_result.value;

    printf("Successfully allocated\n");

    SEA_Result(SEA_VoidPtr) result = SEA_LinearAllocator_alloc_resize_if_needed(&linear_allocator, 4);

    SEA_defer(FILE* f = fopen("my_file", "w"), fclose(f))
    {
        
    }

    if (result.err != NULL)
    {
        SEA_ErrRef_print(result.err);
        SEA_LinearAllocator_free(&linear_allocator);
        return -1;
    }

    return 0;
}