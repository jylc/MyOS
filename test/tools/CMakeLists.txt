project(TestTools)

# 设置print的测试
set(PrintSourceFiles ${PROJECT_SOURCE_DIR}/test_print.cpp)
add_executable(test_print ${PrintSourceFiles})

target_include_tools_header_files(test_print)
# 链接库文件
target_link_libraries(test_print gtest gtest_main test_tools_lib)
add_test(NAME test_print COMMAND test_print)