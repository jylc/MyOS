#include "lib/string.h"

void* memset(void* dest, int val, size_t len) {
	uint8_t* ptr = (uint8_t*)dest;
	while (len-- > 0)
		*ptr++ = val;
	return ptr;
}
void bzero(void* buf, uint32_t len) {
	memset(buf, 0, len);
}