project(tools)

# 寻找CXX文件
aux_source_directory(${tools_SOURCE_DIR}/. tools_src)

# 添加子模块
add_library(${PROJECT_NAME} OBJECT ${tools_src})

# 寻找头文件
target_include_tools_header_files(${PROJECT_NAME}) 
if(test_flag)
	set(CMAKE_CXX_FLAGS " -m32")
	set(CMAKE_C_FLAGS " -m32")
	set(CMAKE_ASM_FLAGS " -m32") 
	add_library(test_tools_lib STATIC ${tools_src})
	target_include_tools_header_files(test_tools_lib)
endif()

