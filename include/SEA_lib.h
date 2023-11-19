#ifndef SEA_LIB
#define SEA_LIB
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

//////////////////////////////////////////////////////////////////////
/*                 SEA Internal Macros and Functions               */
#define __SEA_token_concat_direct(t1, t2) t1 ## t2
#define __SEA_token_concat(t1, t2) __SEA_token_concat_direct(t1, t2)
#define __SEA_macro_var(name) __SEA_token_concat(name, __LINE__)

//////////////////////////////////////////////////////////////////////
//*                 SEA concrete struct declarations              *//

typedef struct SEA_String
{
    unsigned char* string;
    size_t length;
} SEA_String;

typedef struct SEA_LinearAllocator
{
    size_t len;
    size_t capacity;
    unsigned char* memory;
} SEA_LinearAllocator;

typedef struct SEA_Error
{
    int error_code;
    const unsigned char* error_message;
} SEA_Error;

typedef const SEA_Error* SEA_ErrRef;

//////////////////////////////////////////////////////////////////////
//*         SEA_Result functions, macros, and type declarations   *//

//Declares a new SEA_Result struct with the value field being of the type passed in. 
//A SEA_Result struct contains a value field containing the actual value for the object or an err field for when an error occurs
#define SEA_Result_declr(type) typedef struct __SEA_token_concat(SEA_Result, type) { type value; SEA_ErrRef err; } __SEA_token_concat(SEA_Result, type);

//Stands in place for the SEA_Result struct type created from SEA_Result_declr
#define SEA_Result(type) __SEA_token_concat(SEA_Result, type)

typedef void* SEA_VoidPtr;

//SEA_Result type declarations
SEA_Result_declr(SEA_VoidPtr);
SEA_Result_declr(SEA_LinearAllocator);

//////////////////////////////////////////////////////////////////////
//*       SEA_Vec typedef, macros, and function declarations      *//

#define SEA_Vec(type) __SEA_token_concat(SEA_Vec, type)

#define SEA_Vec_declr(type) \
    typedef struct SEA_Vec(type) \
    { \
        size_t len; \
        size_t capacity; \
        type* arr; \
        SEA_LinearAllocator* allocator; \
    } SEA_Vec(type); \
    SEA_Result_declr(SEA_Vec(type)); \
    SEA_Result(SEA_Vec(type)) SEA_Vec##type##_new(SEA_LinearAllocator* const allocator); \
    SEA_Result(type) SEA_Vec##type##_at(const SEA_Vec(type)* const restrict SEA_vec, const size_t idx); \
    SEA_ErrRef SEA_Vec##type##_push_back(SEA_Vec(type)* const restrict SEA_vec, type elem); \
    SEA_ErrRef SEA_Vec##type##_pop_back(SEA_Vec(type)* const restrict SEA_vec); \
    const size_t SEA_Vec##type##_size(const SEA_Vec(type)* const restrict SEA_vec); \
    const type* const SEA_Vec##type##_begin(const SEA_Vec(type)* const restrict SEA_vec); \
    const type* const SEA_Vec##type##_end(const SEA_Vec(type)* const restrict SEA_vec); \
    SEA_ErrRef SEA_Vec##type##_extend(SEA_Vec(type)* dest, SEA_Vec(type)* src); \
    void SEA_Vec##type##_free(SEA_Vec(type)* const restrict SEA_vec); \

//////////////////////////////////////////////////////////////////////
//*              SEA_Error functions and macros                    *//

//Creates a new SEA_Error object from a string literal passed in, along with a name for a variable that references the literal
//This MUST BE ON ITS OWN LINE because it creates a static unsigned char array to reference the literal
#define SEA_Error_new_from_static_string_literal(error_object_name, err_code, string_literal, string_literal_reference_name) \
    static unsigned char string_literal_reference_name[sizeof(string_literal)] = string_literal; \
    SEA_Error error_object_name = { err_code, (unsigned char*)string_literal_reference_name }

#define SEA_ErrRef_new(err_code, string_literal) &(static SEA_Error) { err_code, (static const char[sizeof(string_literal)]){ string_literal } }
#define SEA_Error_new(err_code, string_literal) (SEA_Error) { err_code, (static const char[sizeof(string_literal)]){ string_literal } }

#ifdef __STDC_VERSION__

    //C23 and above
    #if __STDC_VERSION__ == 202311L
       
    #endif

#endif

void SEA_ErrRef_print(SEA_ErrRef err);

//Checks if there was an error or not (if the error code is not 0) and returns if there the result_object if there was an error
//Note: the result object should NOT be an rvalue
#define SEA_try_error(result_object) if (result_object.err != NULL) return result_object;
#define SEA_try(result_type_if_failed, result_object) result_object.value; if (result_object.err != NULL) return (SEA_Result(result_type_if_failed)) { (result_type_if_failed){ 0 }, result_object.err }
#define SEA_try_func(value_object, result_type_if_failed, result_function) auto __SEA_macro_var(_r_) = result_function; if (__SEA_macro_var(_r_).err != NULL) return (SEA_Result(result_type_if_failed)) { (result_type_if_failed){ 0 }, __SEA_macro_var(_r_).err };  value_object = __SEA_macro_var(_r_).value

//////////////////////////////////////////////////////////////////////
//*             Wrappers for standard allocation functions        *//

SEA_Result(SEA_VoidPtr) SEA_malloc(size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_realloc(SEA_VoidPtr ptr, size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_calloc(size_t num_of_objects, size_t size_of_one_object);

//////////////////////////////////////////////////////////////////////
//*         SEA_LinearAllocator functions and macros               *//
SEA_Result(SEA_LinearAllocator) SEA_LinearAllocator_new(size_t size_of_memory_block);
void SEA_LinearAllocator_free (SEA_LinearAllocator* const allocator);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc(SEA_LinearAllocator* const allocator, size_t size_of_memory_block);
SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc_resize_if_needed(SEA_LinearAllocator* const allocator, size_t size_of_memory_block);
void SEA_LinearAllocator_reset(SEA_LinearAllocator* const allocator);

//////////////////////////////////////////////////////////////////////
//*         Quality of life/syntactic macros                      *//

#define SEA_defer(open, close) int __SEA_macro_var(_i_) = 0; for (open; __SEA_macro_var(_i_) < 1; (__SEA_macro_var(_i_)++), close)

//////////////////////////////////////////////////////////////////////
//*              SEA_lib implementation starts here               *//

#ifdef SEA_LIB_IMPLEMENTATION

//////////////////////////////////////////////////////////////////////
//*              SEA_ErrRef functions and implementation           *//

void SEA_ErrRef_print(SEA_ErrRef err)
{
    fprintf(stderr, "Error Code: %d, Error Message: %s", err->error_code, err->error_message);
}

//////////////////////////////////////////////////////////////////////
//*              standard library wrappers                        *//

SEA_Result(SEA_VoidPtr) SEA_malloc(size_t size_of_memory_block)
{
    SEA_VoidPtr ptr = malloc(size_of_memory_block);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = NULL };
    if (!ptr)
    {
        result.err = SEA_ErrRef_new(-1, "Malloc returned null: allocation failed.");
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_realloc(SEA_VoidPtr ptr, size_t size_of_memory_block)
{
    ptr = realloc(ptr, size_of_memory_block);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = NULL };
    if (!ptr)
    {
        result.err = SEA_ErrRef_new(-1, "Realloc returned null: re-allocation failed");
    }
    return result;
}

SEA_Result(SEA_VoidPtr) SEA_calloc(size_t num_of_objects, size_t size_of_one_object)
{
    SEA_VoidPtr ptr = calloc(num_of_objects, size_of_one_object);
    SEA_Result(SEA_VoidPtr) result = { .value = ptr, .err = NULL };
    if (!ptr)
    {
        result.err = SEA_ErrRef_new(-1, "Calloc returned null: allocation failed.");
    }
    return result;
}

//////////////////////////////////////////////////////////////////////
//*         SEA_LinearAllocator implementation starts here              *//

SEA_Result(SEA_LinearAllocator) SEA_LinearAllocator_new(size_t size_of_memory_block)
{
    SEA_Result(SEA_LinearAllocator) result = { 0 };

    result.value.memory = (unsigned char*) malloc(size_of_memory_block);

    if (!result.value.memory)
    {
        result.err = SEA_ErrRef_new(-1, "Malloc returned null for the memory block: allocation failed.");
        return result;
    }

    result.value.len = 0;
    result.value.capacity = size_of_memory_block;

    return result;
}

void SEA_LinearAllocator_free (SEA_LinearAllocator* const allocator)
{
    free(allocator->memory);
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc(SEA_LinearAllocator* const allocator, size_t size_of_memory_block)
{
    SEA_Result(SEA_VoidPtr) result = { .value = NULL, .err = 0 };

    size_t new_allocation_position = allocator->len + size_of_memory_block;

    new_allocation_position += (new_allocation_position % sizeof(void*));

    if (new_allocation_position > allocator->capacity)
    {
        result.err = SEA_ErrRef_new(-1, "The allocation requested exceeds the remaining space in the allocator.");
    }

    allocator->len = new_allocation_position;

    result.value = allocator->memory + new_allocation_position;

    return result;
}

SEA_Result(SEA_VoidPtr) SEA_LinearAllocator_alloc_resize_if_needed(SEA_LinearAllocator* const allocator, size_t size_of_memory_block)
{
    SEA_Result(SEA_VoidPtr) result = { .value = NULL, .err = NULL };

    size_t new_allocation_position = allocator->len + size_of_memory_block;

    new_allocation_position += (new_allocation_position % sizeof(void*));

    if (new_allocation_position > allocator->capacity)
    {
        result = SEA_realloc(allocator->memory, new_allocation_position * 2);
        if (result.err != NULL)
        {
            result.err = SEA_ErrRef_new(-1, "The linear allocator could not be resized: realloc returned null.");
            return result;
        }
        allocator->capacity = new_allocation_position * 2;
    }
    
    result.value = allocator->memory + allocator->len;

    allocator->len = new_allocation_position;

    return result;
}

void SEA_LinearAllocator_reset(SEA_LinearAllocator* const allocator)
{
    allocator->len = 0;
}

//////////////////////////////////////////////////////////////////////
//*              SEA_Vec implementation starts here               *//

#define SEA_Vec_impl(type) \
    SEA_Result(SEA_Vec(type)) SEA_Vec##type##_new(SEA_LinearAllocator* const allocator) \
    { \
        SEA_Result(SEA_Vec(type)) result = { 0 }; \
        SEA_Result(SEA_VoidPtr) malloc_result = SEA_malloc(sizeof(type) * 2); \
        if (malloc_result.err != NULL) \
        { \
            result.err = malloc_result.err; \
            return result; \
        } \
        SEA_VoidPtr arr = malloc_result.value; \
        result.value = (SEA_Vec(type)){ .len = 0, .capacity = 2, .arr = arr }; \
        return result; \
    } \
    SEA_Result(type) SEA_Vec##type##_at(const SEA_Vec(type)* const restrict SEA_vec, const size_t idx) \
    { \
        SEA_Result(type) result = { .value = (type){ 0 }, .err = NULL }; \
        if (idx >= SEA_vec->len) \
        { \
            result.err = SEA_ErrRef_new(-1, "Accesed SEA_Vec out of bounds."); \
            return result; \
        } \
        result.value = SEA_vec->arr[idx]; \
        return result; \
    } \
    SEA_ErrRef SEA_Vec##type##_push_back(SEA_Vec(type)* const restrict SEA_vec, type elm) \
    { \
        if (SEA_vec->len + 1 > SEA_vec->capacity) \
        { \
            SEA_vec->capacity *= 2; \
            SEA_Result(SEA_VoidPtr) realloc_result = { .value = NULL, .err = NULL }; \
            if (SEA_vec->allocator) \
            { \
                realloc_result = SEA_LinearAllocator_alloc_resize_if_needed(SEA_vec->allocator, sizeof(type) * SEA_vec->capacity); \
            } \
            else \
            { \
                realloc_result = SEA_realloc(SEA_vec->arr, sizeof(type) * SEA_vec->capacity); \
            } \
            if (realloc_result.err != NULL) \
            { \
                return realloc_result.err; \
            } \
            SEA_vec->arr = (type*) realloc_result.value; \
        } \
        SEA_vec->arr[SEA_vec->len] = elm; \
        SEA_vec->len++; \
        return NULL; \
    } \
    SEA_ErrRef SEA_Vec##type##_pop_back(SEA_Vec(type)* const restrict SEA_vec) \
    { \
        if (SEA_vec->len <= 0) { return SEA_ErrRef_new(-1, "Cannot pop back from a vector of size 0."); } \
        SEA_vec->len--; \
        return NULL; \
    } \
    const size_t SEA_Vec##type##_size(const SEA_Vec(type)* const restrict SEA_vec) \
    { \
        return SEA_vec->len; \
    } \
    type* const SEA_Vec##type##_iter_begin(const SEA_Vec(type)* const restrict SEA_vec) \
    { \
        return SEA_vec->arr; \
    } \
    type* const SEA_Vec##type##_iter_end(const SEA_Vec(type)* const restrict SEA_vec) \
    { \
        return SEA_vec->arr + SEA_vec->len; \
    } \
    type* const SEA_Vec##type##_iter_incr(type* iter) \
    { \
        return ++iter; \
    } \
    type* const SEA_Vec##type##_iter_decr (type* iter) \
    { \
        return --iter; \
    } \
    SEA_ErrRef SEA_Vec##type##_extend(SEA_Vec(type)* dest, SEA_Vec(type)* src) \
    { \
        if (dest->capacity < dest->len + src->len) \
        { \
            SEA_Result(SEA_VoidPtr) realloc_result = SEA_realloc(dest->arr , (dest->len + src->len) * sizeof(type)); \
            if (realloc_result.err != NULL) \
            { \
                return realloc_result.err; \
            } \
            dest->arr = (type*) realloc_result.value; \
            dest->capacity = dest->len + src->len; \
        } \
        memcpy(dest->arr + dest->len, src->arr, src->len * sizeof(type)); \
        dest->len += src->len; \
        return NULL; \
    } \
    void SEA_Vec##type##_free(SEA_Vec(type)* const restrict SEA_vec) \
    { \
        free(SEA_vec->arr); \
    } \

#endif

#endif
