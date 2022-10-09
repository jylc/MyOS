#include "transform.h"

namespace myos {
	int8_t abs(int8_t num) {
		if (num < 0) {
			return -num;
		}
		return num;
	}

	// int8_t2str int8_t -128-127
	void int8_t2str(int8_t num, char** str) {
		uint8_t index;
		if (num < 0) {
			num = abs(num);
			*str = (char *)"00000";
			(*str)[0] = '-';
			(*str)[1] = '0';
			(*str)[2] = '0';
			(*str)[3] = '0';
			(*str)[4] = '\0';
			index = 3;
		}
		else {
			*str = (char *)"0000";
			index = 3;
			(*str)[0] = '0';
			(*str)[1] = '0';
			(*str)[2] = '0';
			(*str)[3] = '\0';
		}
		while (num != 0)
		{
			(*str)[index--] = num % 10 + '0';
			num /= 10;
		}
	}

	// uint8_t2str uint8_t 0-255
	void uint8_t2str(uint8_t num, char** str) {
		uint8_t index;
		char arr[4];
		*str = arr;
		index = 2;
		(*str)[0] = '0';
		(*str)[1] = '0';
		(*str)[2] = '0';
		(*str)[3] = '\0';

		while (num != 0)
		{
			(*str)[index--] = num % 10 + '0';
			num /= 10;
		}
	}
}