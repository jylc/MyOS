# 设置子模块
project(kernel CXX)

# 寻找CXX文件
aux_source_directory(${kernel_SOURCE_DIR}/. kernel_src)

# 添加子模块
add_library(${PROJECT_NAME} OBJECT ${kernel_src})

target_include_kernel_header_files(${PROJECT_NAME})