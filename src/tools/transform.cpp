#include "transform.h"

namespace myos {
	int8_t abs(int8_t num) {
		if (num < 0) {
			return -num;
		}
		return num;
	}
}