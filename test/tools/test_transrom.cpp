#include "gtest/gtest.h"
#include "transform.h"
TEST(TestAddInt, test_add_int) {
	myos::uint8_t unum = 250;
	char* str;
	myos::uint8_t2str(unum, &str);
	EXPECT_STREQ(str, "0250");
}