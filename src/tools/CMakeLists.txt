project(tools)

# 寻找CXX文件
aux_source_directory(${tools_SOURCE_DIR}/. tools_src)
aux_source_directory(${tools_SOURCE_DIR}/lib tools_src_lib)


# 添加子模块
add_library(${PROJECT_NAME} OBJECT ${tools_src} ${tools_src_lib})
# 寻找头文件
target_include_tools_header_files(${PROJECT_NAME}) 

add_library(test_tools_lib STATIC ${tools_src} ${tools_src_lib})
target_include_tools_header_files(test_tools_lib)
