#include <stdio.h>
#include <stdbool.h>
#include "../include/SEA_lib.h"

#define SEA_eval(x, ...) x

#define SEA_Str_Enum(...) 

enum SEA_Err {
    Ok,
    NoValueError,
    IdxError,
    ArithmeticError,
    ReturnError,
    MallocError,
};

static const char *const SEA_ERROR_MESSAGES[] = {
    "No Error",
    "No Value",
    "Indexing Error",
    "Arithmetic Error",
    "Return Error",
    "Malloc Error",
};

#define SEA_Result_declr(type) \
    typedef struct \
    { \
        union \
        { \
            type Ok; \
            enum SEA_Err Err; \
        }; \
        bool isErr; \
    } __SEA_token_concat(SEA_Result_, type);

#define SEA_Result_impl(type) \
    SEA_Result(type) __SEA_token_concat(SEA_Result_from_, type)(type from) \
    { \
        return (SEA_Result(type)) { .Ok = from, .isErr = false }; \
    } \
    type __SEA_token_concat(SEA_Result_unwrap_, type)(SEA_Result(type) from) \
    { \
        if (!from.isErr) return from.Ok; \
        __builtin_trap(); \
    } \
    type __SEA_token_concat(SEA_Result_unwrap_or_, type)(SEA_Result(type) from, type fallback) \
    { \
        return from.isErr ? fallback : from.Ok; \
    } \
    SEA_Result(type) __SEA_token_concat(SEA_Result_from_err_, type)(enum SEA_Err err) \
    { \
        return (SEA_Result(type)) { .Err = err, .isErr = true }; \
    }

#define SEA_Result(type)            __SEA_token_concat(SEA_Result_, type)
#define SEA_Result_from(type)       __SEA_token_concat(SEA_Result_from_, type)
#define SEA_Result_from_err(type)   __SEA_token_concat(SEA_Result_from_err_, type)
#define SEA_Result_unwrap(type)     __SEA_token_concat(SEA_Result_unwrap_, type)
#define SEA_Result_unwrap_or(type)  __SEA_token_concat(SEA_Result_unwrap_or_, type)

#define SEA_PtrResult_declr(ptr_type) \
    typedef struct \
    { \
        union \
        { \
            ptr_type *Ok; \
            enum SEA_Err Err; \
        }; \
        bool isErr; \
    } __SEA_token_concat(SEA_PtrResult_, ptr_type);

#define SEA_PtrResult_impl(ptr_type) \
    SEA_PtrResult(ptr_type) __SEA_token_concat(SEA_PtrResult_from_, ptr_type)(ptr_type from) \
    { \
        return (SEA_PtrResult(ptr_type)) { .Ok = from, .isErr = false }; \
    } \
    ptr_type *__SEA_token_concat(SEA_PtrResult_unwrap_, ptr_type)(SEA_PtrResult(ptr_type) from) \
    { \
        if (!from.isErr) return from.Ok; \
        __builtin_trap(); \
    } \
    ptr_type *__SEA_token_concat(SEA_PtrResult_unwrap_or_, ptr_type)(SEA_PtrResult(ptr_type) from, ptr_type *fallback) \
    { \
        return from.isErr ? fallback : from.Ok; \
    } \
    SEA_PtrResult(ptr_type) __SEA_token_concat(SEA_PtrResult_from_err_, ptr_type)(enum SEA_Err err) \
    { \
        return (SEA_PtrResult(ptr_type)) { .Err = err, .isErr = true }; \
    } \

#define SEA_PtrResult(type)            __SEA_token_concat(SEA_PtrResult_, type)
#define SEA_PtrResult_from(type)       __SEA_token_concat(SEA_PtrResult_from_, type)
#define SEA_PtrResult_from_err(type)   __SEA_token_concat(SEA_PtrResult_from_err_, type)
#define SEA_PtrResult_unwrap(type)     __SEA_token_concat(SEA_PtrResult_unwrap_, type)
#define SEA_PtrResult_unwrap_or(type)  __SEA_token_concat(SEA_PtrResult_unwrap_or_, type)

#define _SEA_Result_unwrap_unchecked(res) (res.Ok)
#define _SEA_Result_unwrap_or(res, fallback) (res.isErr ? fallback : res.Ok)
#define _breakcase break; case
#define _if_Ok _breakcase NoErr

#define _SEA_Result_rvalue_match(res, name) \
    for (auto name = res, __SEA_macro_var(__run_once__) = (typeof(name)) { Err, true }; __SEA_macro_var(__run_once__).isErr; __SEA_macro_var(__run_once__).isErr = false) \
        switch (name.Err)

#define SEA_Result_match(res) switch (res.Err)

#define Statement(statement) do { statement ;} while(false)

SEA_Result_declr(int)
SEA_Result_impl(int)


int main(int argc, char *argv[])
{
    SEA_Result(int) result = SEA_Result_from(int)(2);

    printf("Result of result: %d", SEA_Result_unwrap_or(int)(result, 3));
}