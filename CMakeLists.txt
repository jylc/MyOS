# ������Сcmake�汾
cmake_minimum_required(VERSION 3.23)

# �������������
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# ������Ŀ��������
project(MYOS LANGUAGES CXX ASM)

# �������԰汾
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)


# ���ø��� cmake �ű�·��
set(CMAKE_MODULE_PATH "${SimpleKernel_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(${MYOS_SOURCE_DIR}/cmake/header_files.cmake)
include(${MYOS_SOURCE_DIR}/cmake/find_asm_files.cmake)

# �������·��
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
# �����ں�����
set(KernelName kernel.bin)

# ����src·��
set(Kernel_SOURCE_CODE_DIR ${MYOS_SOURCE_DIR}/src)

# ����include·��
set(Kernel_INCLUDE_CODE_DIR ${MYOS_SOURCE_DIR}/include)
message(STATUS ${MYOS_PROJECT_NAME})
# ��ת
add_subdirectory(${Kernel_SOURCE_CODE_DIR})
