#include <stdio.h>
#include <stdbool.h>
#include "include/SEA_lib.h"

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

#define _SEA_Result_declr(type) \
    typedef struct __SEA_token_concat(SEA_Result_, type) { \
        union \
        { \
            type Ok; \
            enum SEA_Err Err; \
        }; \
        bool isErr; \
    } __SEA_token_concat(SEA_Result_, type);

#define _SEA_Result(type) __SEA_token_concat(SEA_Result_, type)
#define _SEA_Result_perror(res) if (res.isErr) puts(SEA_ERROR_MESSAGES[res.Err])
#define _SEA_Result_failed_unwrap(res) *(typeof(res.Ok)*)0
#define _SEA_Result_unwrap_or(res, default) (res.isErr ? default : res.Ok)
#define _SEA_Result_unwrap(res) (res.isErr ? _SEA_Result_failed_unwrap(res) : res.Ok)

_SEA_Result_declr(float)

_SEA_Result(float) retErrResult(bool shouldRetErr)
{
    return shouldRetErr ? (_SEA_Result(float)) { .Err = NoValueError, .isErr = true } : (_SEA_Result(float)) { 3.0f };
}

int main()
{
    _SEA_Result(float) res = retErrResult(true);
    _SEA_Result_perror(res);

    int x = _SEA_Result_unwrap_or(res, 2);

    printf("%d", x);
}