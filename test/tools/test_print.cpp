#include "gtest/gtest.h"
#include "print.h"
#include <iostream>
using namespace std;
TEST(TestPrintf, test_printf) {
	char *str=myos::tools::printf_test("hello:%s\n","world");
	std::printf(str);
	str = myos::tools::printf_test("hello:%lx\n", 12);
	std::printf(str);
	//cout << "Hello world" << endl;
	//EXPECT_EQ(str, "hello");
}