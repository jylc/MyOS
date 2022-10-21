#ifndef MYOS_STRING_H
#define MYOS_STRING_H
#include "../../common/types.h"
void* memset(void* dest, int val, myos::size_t len);
void bzero(void* buf, myos::uint32_t len);



#endif // !MYOS_STRING_H
