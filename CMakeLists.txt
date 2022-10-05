# 设置最小cmake版本
cmake_minimum_required(VERSION 3.23)

# 跳过编译器检查
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# 设置项目名与语言
project(MYOS LANGUAGES CXX ASM)

# 设置语言版本
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)


# 设置辅助 cmake 脚本路径
set(CMAKE_MODULE_PATH "${SimpleKernel_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(${MYOS_SOURCE_DIR}/cmake/header_files.cmake)
include(${MYOS_SOURCE_DIR}/cmake/find_asm_files.cmake)

# 设置输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
# 设置内核名称
set(KernelName kernel.bin)

# 设置src路径
set(Kernel_SOURCE_CODE_DIR ${MYOS_SOURCE_DIR}/src)

# 设置include路径
set(Kernel_INCLUDE_CODE_DIR ${MYOS_SOURCE_DIR}/include)
message(STATUS ${MYOS_PROJECT_NAME})
# 跳转
add_subdirectory(${Kernel_SOURCE_CODE_DIR})

