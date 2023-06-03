#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef int32_t i32;

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

#define OK    0
#define ERROR 1

#define EXIT()                                              \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        _exit(ERROR);                                       \
    } while (FALSE)

#define EXIT_WITH(x)                                                \
    do {                                                            \
        printf("%s:%s:%d `%s`\n", __FILE__, __func__, __LINE__, x); \
        _exit(ERROR);                                               \
    } while (FALSE)

#define EXIT_IF(condition)         \
    do {                           \
        if (condition) {           \
            EXIT_WITH(#condition); \
        }                          \
    } while (FALSE)

#endif
