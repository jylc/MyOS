project(net)

# 寻找CXX文件
aux_source_directory(${net_SOURCE_DIR}/. net_src)

# 添加子模块
add_library(${PROJECT_NAME} OBJECT ${net_src})

# 寻找头文件
target_include_header_files(${PROJECT_NAME})
target_include_common_header_files(${PROJECT_NAME})
target_include_tools_header_files(${PROJECT_NAME})
target_include_gui_header_files(${PROJECT_NAME})