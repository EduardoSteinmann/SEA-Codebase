#ifndef SEA_LIB
#define SEA_LIB
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
    const unsigned char* error_message;
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
SEA_Result_declr(SEA_LinearAllocator);

//////////////////////////////////////////////////////////////////////
//*              SEA_Error functions and macros                    *//

//Creates a new SEA_Error object from a string literal passed in, along with a name for a variable that references the literal
//This MUST BE ON ITS OWN LINE because it creates a static unsigned char array to reference the literal
#define SEA_Error_new_from_static_string_literal(error_object_name, err_code, string_literal, string_literal_reference_name) \
    static unsigned char string_literal_reference_name[sizeof(string_literal)] = string_literal; \
    SEA_Error error_object_name = { err_code, (unsigned char*)string_literal_reference_name }

#define SEA_Error_new(err_code, string_literal) (SEA_Error) { err_code, (static const char[sizeof(string_literal)]){ string_literal } }

void SEA_Error_print(SEA_Error err);

//Checks if there was an error or not (if the error code is not 0) and returns if there the result_object if there was an error
//Note: the result object should NOT be an rvalue
#define SEA_try_error(result_object) if (result_object.err.error_code != 0) return result_object;
#define SEA_try(result_type_if_failed, result_object) result_object.value; if (result_object.err.error_code != 0) return (SEA_Result(result_type_if_failed)) { (result_type_if_failed){ 0 }, result_object.err }
#define SEA_try_func(value_object, result_type_if_failed, result_function) auto __SEA_macro_var(_r_) = result_function; if (__SEA_macro_var(_r_).err.error_code != 0) return (SEA_Result(result_type_if_failed)) { (result_type_if_failed){ 0 }, __SEA_macro_var(_r_).err };  value_object = __SEA_macro_var(_r_).value

//////////////////////////////////////////////////////////////////////
//*             Wrappers for standard allocation functions        *//

SEA_Result(SEA_VoidPtr) SEA_malloc(size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_realloc(SEA_VoidPtr ptr, size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_calloc(size_t num_of_objects, size_t size_of_one_object);

//////////////////////////////////////////////////////////////////////
//*         SEA_LinearAllocator functions and macros               *//
SEA_Result(SEA_LinearAllocator) SEA_LinearAllocator_new(size_t size_of_memory_block);
void SEA_LinearAllocator_free (SEA_LinearAllocator* const linear_allocator);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc(SEA_LinearAllocator* const allocator, size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc_resize_allocator_if_needed(SEA_LinearAllocator* const allocator, size_t size_of_memory_block);
void SEA_LinearAllocator_reset_allocation_position(SEA_LinearAllocator* const allocator);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_realloc(SEA_LinearAllocator* const allocator, SEA_VoidPtr* ptr, size_t current_size, size_t desired_size);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_realloc_resize_allocator_if_needed(SEA_LinearAllocator* const allocator, SEA_VoidPtr* ptr, size_t current_size, size_t desired_size);

//////////////////////////////////////////////////////////////////////
//*         Quality of life/syntactic macros                      *//

#define SEA_defer(open, close) int __SEA_macro_var(_i_) = 0; for (open; __SEA_macro_var(_i_) < 1; (__SEA_macro_var(_i_)++), close)

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
        result.err = SEA_Error_new(-1, "Malloc returned null: allocation failed.");
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_realloc(SEA_VoidPtr ptr, size_t size_of_memory_block)
{
    ptr = realloc(ptr, size_of_memory_block);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = { .error_code = 0, .error_message = NULL } };
    if (!ptr)
    {
        result.err = SEA_Error_new(-1, "Realloc returned null: re-allocation failed");
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_calloc(size_t num_of_objects, size_t size_of_one_object)
{
    SEA_VoidPtr ptr = calloc(num_of_objects, size_of_one_object);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = { .error_code = 0, .error_message = NULL } };
    if (!ptr)
    {
        result.err = SEA_Error_new(-1, "Calloc returned null: allocation failed.");
    }
    return result;
}

SEA_Result(SEA_LinearAllocator) SEA_LinearAllocator_new(size_t size_of_memory_block)
{
    SEA_LinearAllocator linear_allocator = { .current_allocation_position = -1, .memory = NULL, .capacity = -1 };

    SEA_Result(SEA_LinearAllocator) result = { .value = linear_allocator, .err = { .error_code = 0, .error_message = NULL } };

    linear_allocator.memory = (unsigned char*) malloc(size_of_memory_block);

    if (!linear_allocator.memory)
    {
        result.err = SEA_Error_new(-1, "Malloc returned null for the memory block: allocation failed.");
        return result;
    }

    linear_allocator.current_allocation_position = 0;
    linear_allocator.capacity = size_of_memory_block;

    return result;
}

void SEA_LinearAllocator_free (SEA_LinearAllocator* const linear_allocator)
{
    free(linear_allocator->memory);
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc(SEA_LinearAllocator* const allocator, size_t size_of_memory_block)
{
    SEA_Result(SEA_VoidPtr) result = { .value = NULL, .err = { .error_code = 0, .error_message = NULL } };

    size_t new_allocation_position = allocator->current_allocation_position + size_of_memory_block;

    new_allocation_position += (new_allocation_position % sizeof(void*));

    if (new_allocation_position > allocator->capacity)
    {
        result.err = SEA_Error_new(-1, "The allocation requested exceeds the remaining room in the allocator.");
    }

    allocator->current_allocation_position = new_allocation_position;

    result.value = allocator->memory + new_allocation_position;

    return result;
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc_resize_if_needed(SEA_LinearAllocator* const allocator, size_t size_of_memory_block)
{
    SEA_Result(SEA_VoidPtr) result = { .value = NULL, .err = { .error_code = 0, .error_message = NULL } };

    size_t new_allocation_position = allocator->current_allocation_position + size_of_memory_block;

    new_allocation_position += (new_allocation_position % sizeof(void*));

    if (new_allocation_position > allocator->capacity)
    {
        result = SEA_realloc(allocator->memory, new_allocation_position * 2);
        if (result.err.error_code != 0)
        {
            result.err = SEA_Error_new(-1, "The linear allocator could not be resized: realloc returned null.");
            return result;
        }
        allocator->capacity = new_allocation_position * 2;
    }
    
    result.value = allocator->memory + allocator->current_allocation_position;

    allocator->current_allocation_position = new_allocation_position;

    return result;
}

void SEA_LinearAllocator_reset(SEA_LinearAllocator* const allocator)
{
    allocator->current_allocation_position = 0;
}

//////////////////////////////////////////////////////////////////////
/*                 SEA Internal Macros and Functions               */
#define __SEA_token_concat_direct(t1, t2) t1 ## t2
#define __SEA_token_concat(t1, t2) __SEA_token_concat_direct(t1, t2)
#define __SEA_macro_var(name) __SEA_token_concat(name, __LINE__)

#endif

#endif