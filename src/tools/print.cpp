#include "print.h"
int add_int(int x, int y) {
	return x + y;
}
namespace myos {
	namespace tools {
		void printf(const char* str) {
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

		/*template<typename T, typename ... Args>
		void printf(const T str, const Args&...args) {
			size_t argsSize = sizeof...(args);
			for (uint8_t i = 0; str[i] != '\0'; i++) {
				if (str[i] == '%') {
					i++;
				}
			}
		}*/
	}
}
