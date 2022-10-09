project(TestTools)

# 设置print的测试
set(PrintSourceFiles ${PROJECT_SOURCE_DIR}/test_print.cpp)
add_executable(test_print ${PrintSourceFiles})

target_include_tools_header_files(test_print)
# 链接库文件
target_link_libraries(test_print gtest gtest_main test_tools_lib)
add_test(NAME test_print COMMAND test_print)

# 设置transform的测试
# 设置print的测试
set(TransformSourceFiles ${PROJECT_SOURCE_DIR}/test_print.cpp)
add_executable(test_transfrom ${PrintSourceFiles})

target_include_tools_header_files(test_transfrom)
# 链接库文件
target_link_libraries(test_transfrom gtest gtest_main test_tools_lib)
add_test(NAME test_transfrom COMMAND test_transfrom)
