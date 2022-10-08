project(tools)

# 寻找CXX文件
aux_source_directory(${tools_SOURCE_DIR}/. tools_src)

# 添加子模块
add_library(${PROJECT_NAME} OBJECT ${tools_src})

# 寻找头文件
target_include_tools_header_files(${PROJECT_NAME})