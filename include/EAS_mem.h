#ifndef SEA_STR
#define SEA_STR
#include <stdint.h>

typedef struct SEA_String
{
    unsigned char* string;
    size_t length;
} SEA_String;

typedef struct SEA_Linear_Allocator 
{
    size_t current_allocation_position;
    size_t capacity;
    unsigned char* memory;
} SEA_Linear_Allocator;

typedef struct SEA_Error
{
    intptr_t error_code;
    SEA_String* error_mesage;
} SEA_Error;

#ifdef SEA_LIB_IMPL

#endif

#endif