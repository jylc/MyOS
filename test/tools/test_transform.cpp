#include "gtest/gtest.h"
#include "transform.h"
TEST(test_transform, test_ab) {
	myos::int8_t num = -25;
	myos::int8_t ans=myos::abs(num);
	ASSERT_EQ(ans, 25);
}