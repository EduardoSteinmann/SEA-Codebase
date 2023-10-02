#ifndef SEA_STR
#define SEA_STR
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

//////////////////////////////////////////////////////////////////////
//*                 SEA struct declarations                        *//

typedef struct SEA_String
{
    unsigned char* string;
    size_t length;
} SEA_String;

typedef struct SEA_LinearAllocator 
{
    size_t current_allocation_position;
    size_t capacity;
    unsigned char* memory;
} SEA_LinearAllocator;

typedef struct SEA_Error
{
    int error_code;
    unsigned char* error_message;
} SEA_Error;

//////////////////////////////////////////////////////////////////////
//*         SEA_Result functions, macros, and type declarations   *//

//Declares a new SEA_Result struct with the value field being of the type passed in. 
//A SEA_Result struct contains a value field containing the actual value for the object or an err field for when an error occurs
#define SEA_Result_declr(type) typedef struct SEA_Result##type { type value; SEA_Error err; } SEA_Result##type;

//Stands in place for the SEA_Result struct type created from SEA_Result_declr
#define SEA_Result(type) SEA_Result##type

typedef void* SEA_VoidPtr;

//SEA_Result type declarations
SEA_Result_declr(SEA_VoidPtr);

//////////////////////////////////////////////////////////////////////
//*              SEA_Error functions and macros                    *//

//Creates a new SEA_Error object from a string literal passed in, along with a name for a variable that references the literal
//This MUST BE ON ITS OWN LINE because it creates a static unsigned char array to reference the literal
#define SEA_Error_new_from_static_string_literal(error_object_name, err_code, string_literal, string_literal_reference_name) \
    static unsigned char string_literal_reference_name[sizeof(string_literal)] = string_literal; \
    SEA_Error error_object_name = { err_code, (unsigned char*)string_literal_reference_name }

void SEA_Error_print(SEA_Error err);

//Checks if there was an error or not (if the error code is not 0) and returns if there the result_object if there was an error
//Note: the result object should NOT be an rvalue
#define SEA_try_error(result_object) if (result_object.err.error_code != 0) return result_object;

//////////////////////////////////////////////////////////////////////
//*             Wrappers for standard allocation functions        *//

SEA_Result(SEA_VoidPtr) SEA_malloc(size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_realloc(SEA_VoidPtr ptr, size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_calloc(size_t num_of_objects, size_t size_of_one_object);

//////////////////////////////////////////////////////////////////////
//*         SEA_LinearAllocator functions and macros               *//
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_new(size_t size_of_memory_block);
void SEA_LinearAllocator_free (SEA_LinearAllocator* linear_allocator);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc(SEA_LinearAllocator* allocator, size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc_resize_allocator_if_needed(SEA_LinearAllocator* allocator, size_t size_of_memory_block);

//////////////////////////////////////////////////////////////////////
//*         SEA_lib implementation starts here                    *//

#ifdef SEA_LIB_IMPLEMENTATION

void SEA_Error_print(SEA_Error err)
{
    printf("Error message: %s\nError Code: %d\n", err.error_message, err.error_code);
}

SEA_Result(SEA_VoidPtr) SEA_malloc(size_t size_of_memory_block)
{
    SEA_VoidPtr ptr = malloc(size_of_memory_block);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = { .error_code = 0, .error_message = NULL } };
    if (!ptr)
    {
        SEA_Error_new_from_static_string_literal(malloc_failed_err, -1, "Malloc returned null: allocation failed.", malloc_failed_message);
        result.err = malloc_failed_err;
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_realloc(SEA_VoidPtr ptr, size_t size_of_memory_block)
{
    ptr = realloc(ptr, size_of_memory_block);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = { .error_code = 0, .error_message = NULL } };
    if (!ptr)
    {
        SEA_Error_new_from_static_string_literal(realloc_failed_err, -1, "Realloc returned null: re-allocation failed, old memory block freed.", realloc_failed_message);
        result.err = realloc_failed_err;
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_calloc(size_t num_of_objects, size_t size_of_one_object)
{
    SEA_VoidPtr ptr = calloc(num_of_objects, size_of_one_object);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = { .error_code = 0, .error_message = NULL } };
    if (!ptr)
    {
        SEA_Error_new_from_static_string_literal(calloc_failed_error, -1, "Calloc returned null: allocation failed.", calloc_failed_message);
        result.err = calloc_failed_error;
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_new(size_t size_of_memory_block)
{
    SEA_LinearAllocator* linear_allocator = (SEA_LinearAllocator*) malloc(sizeof(SEA_LinearAllocator));
    SEA_Result(SEA_VoidPtr) result = { .value = linear_allocator, .err = { .error_code = 0, .error_message = NULL } };

    if (!linear_allocator)
    {
        SEA_Error_new_from_static_string_literal(linear_allocator_err, -1, "Malloc returned null for the pointer to the SEA_LinearAllocator.", allocator_error_message);
        result.err = linear_allocator_err;
        return result;
    }

    linear_allocator->memory = (unsigned char*) malloc(size_of_memory_block);

    if (!linear_allocator->memory)
    {
        SEA_Error_new_from_static_string_literal(memory_block_err, -1, "Malloc returned null for the block of memory for the allocator.", malloc_err_message);
        result.err = memory_block_err;
        result.value = NULL;
        free(linear_allocator);
        return result;
    }

    linear_allocator->current_allocation_position = 0;
    linear_allocator->capacity = size_of_memory_block;

    return result;
}

void SEA_LinearAllocator_free (SEA_LinearAllocator* linear_allocator)
{
    free(linear_allocator->memory);
    free(linear_allocator);
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc(SEA_LinearAllocator* allocator, size_t size_of_memory_block)
{
    SEA_Result(SEA_VoidPtr) result = { .value = NULL, .err = { .error_code = 0, .error_message = NULL } };

    size_t new_allocation_position = allocator->current_allocation_position + size_of_memory_block;

    new_allocation_position += (new_allocation_position % sizeof(void*));

    if (new_allocation_position > allocator->capacity)
    {
        SEA_Error_new_from_static_string_literal(capacity_exceeded_err, -1, "The allocation requested exceeds the remaining room in the allocator.", capacity_exceeded_message);
        result.err = capacity_exceeded_err;
    }

    allocator->current_allocation_position = new_allocation_position;

    result.value = allocator->memory + new_allocation_position;

    return result;
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc_resize_allocator_if_needed(SEA_LinearAllocator* allocator, size_t size_of_memory_block)
{
    SEA_Result(SEA_VoidPtr) result = { .value = NULL, .err = { .error_code = 0, .error_message = NULL } };

    size_t new_allocation_position = allocator->current_allocation_position + size_of_memory_block;

    new_allocation_position += (new_allocation_position % sizeof(void*));

    if (new_allocation_position > allocator->capacity)
    {
        result = SEA_realloc(allocator->memory, new_allocation_position * 2);
        if (result.err.error_code != 0)
        {
            SEA_Error_new_from_static_string_literal(realloc_failed_err, -1, "The linear allocator could not be resized: realloc returned null.", realloc_failed_message);
            result.err = realloc_failed_err;
            return result;
        }
        allocator->capacity = new_allocation_position * 2;
    }
    
    result.value = allocator->memory + allocator->current_allocation_position;

    allocator->current_allocation_position = new_allocation_position;

    return result;
}

#endif

#endif