#include "print.h"
namespace myos {
	namespace tools {
		void printf(const char* _fmt, ...) {
			va_list_ va;
			va_start(va, _fmt);
			const int ret = _vsnprintf(buf, BUF_SIZE, _fmt, va);
			va_end(va);
			printf_screen(buf);
			bzero(buf, BUF_SIZE);
		}

		char* printf_test(const char* _fmt, ...) {
			va_list_ va;
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
					/*clear_screen();
					x = 0;
					y = 0;*/
					scroll(x, y);
				}
			}
		}


		void printfHex(const char* str, uint8_t key) {
			printf(str, key);
		}

		void clear_screen() {
			uint8_t attribyte_byte = (0 << 4) | (15 & 0x0f);
			uint16_t blank = 0x20 | (attribyte_byte << 8);
			uint16_t* VideoMemory = (uint16_t*)0xb8000;
			for (uint8_t y = 0; y < 25; y++) {
				for (uint8_t x = 0; x < 80; x++) {
					VideoMemory[80 * y + x] = blank;
				}
			}
		}

		void scroll(uint8_t&x, uint8_t&y) {
			uint8_t attribyte_byte = (0 << 4) | (15 & 0x0f);
			uint16_t blank = 0x20 | (attribyte_byte << 8);
			uint16_t* VideoMemory = (uint16_t*)0xb8000;
			if (y >= 25) {
				for (uint8_t y_t = 0; y_t < 24; y_t++) {
					for (uint8_t x_t = 0; x_t < 80; x_t++) {
						VideoMemory[80 * y_t + x_t] = VideoMemory[80 * (y_t +1) + x_t];
					}
				}

				for (uint8_t x_t = 0; x_t < 80; x_t++) {
					VideoMemory[80 * 25 + x_t] = blank;
				}
				y--;
			}
		}
	}
}
