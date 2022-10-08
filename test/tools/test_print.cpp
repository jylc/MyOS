#include "gtest/gtest.h"
#include "print.h"
TEST(TestAddInt, test_add_int) {
	int res = add_int(10, 24);
	EXPECT_EQ(res, 34);
}