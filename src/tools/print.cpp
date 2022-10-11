#include "print.h"
namespace myos {
	namespace tools {
		void printf(const char* _fmt, ...) {
			va_list va;
			va_start(va, _fmt);
			const int ret = _vsnprintf(buf, BUF_SIZE, _fmt, va);
			va_end(va);
			printf_screen(buf);
			bzero(buf, BUF_SIZE);
		}

		char* printf_test(const char* _fmt, ...) {
			va_list va;
			va_start(va, _fmt);
			const int ret = _vsnprintf(buf, BUF_SIZE, _fmt, va);
			va_end(va);
			return buf;
		}



		void printf_screen(const char* str) {
			static uint16_t* VideoMemory = (uint16_t*)0xb8000;
			static uint8_t x = 0, y = 0;
			for (int i = 0; str[i] != '\0'; ++i) {
				switch (str[i]) {
				case '\n':
					y++;
					x = 0;
					break;
				default:
					VideoMemory[80 * y + x] = (VideoMemory[i] & 0xFF00) | str[i];
					x++;
					break;
				}

				if (x >= 80) {
					x = 0;
					y++;
				}
				if (y >= 25) {
					for (y = 0; y < 25; y++) {
						for (x = 0; x < 80; x++) {
							VideoMemory[x * y] = (VideoMemory[x * y] & 0xFF00) | ' ';
						}
					}
					x = 0;
					y = 0;
				}
			}
		}

		

		void printfHex(const char* str, uint8_t key) {
			printf(str, key);
		}
	}
}
