#ifndef MYOS_PRINT_H
#define MYOS_PRINT_H
#include "../common/types.h"
#include "lib/stdarg.h"
#include "lib/stdio.h"
#include "lib/string.h" 
namespace myos {
	namespace tools {
		static constexpr const size_t BUF_SIZE = 128;
		static char buf[BUF_SIZE];
		void printf_screen(const char* str);
		void printf(const char* _fmt, ...);
		void printfHex(char* str, uint8_t key);
		char* printf_test(const char* _fmt, ...);
	}
}

#endif // !MYOS_PRINT_H