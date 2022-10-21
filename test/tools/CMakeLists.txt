project(TestTools)

set(TestToolsFiles
	test_print
	test_transform
)

foreach(src ${TestToolsFiles})
	add_executable(${src} EXCLUDE_FROM_ALL ${TestTools_SOURCE_DIR}/${src}.cpp)
	message(STATUS "test file:${TestTools_SOURCE_DIR}/${src}.cpp")
	target_include_tools_header_files(${src})
	target_link_libraries(${src} gtest gtest_main test_tools_lib)
	add_test(NAME ${src} COMMAND ${src})
endforeach()
