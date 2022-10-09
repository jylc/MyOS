#ifndef MYOS_STDIO_H
#define MYOS_STDIO_H
#include "../../common/types.h"
using namespace myos;

#define PRINTF_NTOA_BUFFER_SIZE 32U

// internal flag definitions
#define FLAGS_ZEROPAD (1U << 0U)
#define FLAGS_LEFT (1U << 1U)
#define FLAGS_PLUS (1U << 2U)
#define FLAGS_SPACE (1U << 3U)
#define FLAGS_HASH (1U << 4U)
#define FLAGS_UPPERCASE (1U << 5U)
#define FLAGS_CHAR (1U << 6U)
#define FLAGS_SHORT (1U << 7U)
#define FLAGS_LONG (1U << 8U)
#define FLAGS_LONG_LONG (1U << 9U)
#define FLAGS_PRECISION (1U << 10U)

typedef void (*out_func_type)(char character, void* buffer, myos::size_t idx, myos::size_t maxLen);

static inline void _out_buffer(char character, void* buffer, myos::size_t idx, myos::size_t maxLen) {
    if (idx < maxLen) {
        ((char*)buffer)[idx] = character;
    }
}

static inline void _null_out_buffer(char character, void* buffer, myos::size_t idx, myos::size_t maxLen) {
    (void)character;
    (void)buffer;
    (void)idx;
    (void)maxLen;
}


static inline bool _is_digit(char ch) {
    return (ch >= '0') && (ch <= '9');
}

static  uint32_t _atoi(const char** str) {
    uint32_t num = 0U;
    while (_is_digit(**str))
    {
        num = num * 10U + (uint32_t)(*((*str)++) - '0');
    }
    return num;
}


// internal itoa for 'long' type
static myos::size_t _ntoa_long(out_func_type out, char* buffer, myos::size_t idx,
    myos::size_t maxlen, unsigned long value, bool negative,
    unsigned long base, unsigned int prec,
    unsigned int width, unsigned int flags);


// internal itoa format
static myos::size_t _ntoa_format(out_func_type out, char* buffer, myos::size_t idx,
    myos::size_t maxlen, char* buf, myos::size_t len, bool negative,
    unsigned int base, unsigned int prec,
    unsigned int width, unsigned int flags);

// output the specified string in reverse, taking care of any zero-padding
static myos::size_t _out_rev(out_func_type out, char* buffer, myos::size_t idx,
    myos::size_t maxlen, const char* buf, myos::size_t len,
    unsigned int width, unsigned int flags);

static inline unsigned int _strnlen_s(const char* str, myos::size_t maxsize);

int _vsnprintf(char* buffer, const myos::size_t maxlen, const char* format,
    va_list va);

#endif // !MYOS_STDIO_H


