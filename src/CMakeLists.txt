
# 设置编译参数
set(CMAKE_CXX_FLAGS "-march=corei7 -mtune=corei7 -m32 -Di386 -m32 -Iinclude -fno-use-cxa-atexit -fleading-underscore -fno-exceptions -fno-builtin -nostdlib -fno-rtti -fno-pie")
set(CMAKE_C_FLAGS "-march=corei7 -mtune=corei7 -m32 -Di386 -m32 -Iinclude -fno-use-cxa-atexit -fleading-underscore -fno-exceptions -fno-builtin -nostdlib -fno-rtti -fno-pie")
set(CMAKE_ASM_FLAGS "-m32") 

message(STATUS "CMAKE_BUILD_TYPE IS ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_FLAGS IS ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS IS ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_ASM_FLAGS IS ${CMAKE_ASM_FLAGS}")


# 子目录下的cmakelists
add_subdirectory(${Kernel_SOURCE_CODE_DIR}/boot)
add_subdirectory(${Kernel_SOURCE_CODE_DIR}/kernel)

add_executable(${KernelName} $<TARGET_OBJECTS:boot> $<TARGET_OBJECTS:kernel>)

target_link_options(${KernelName} PRIVATE -T ${Kernel_SOURCE_CODE_DIR}/linker.ld)
target_link_options(${KernelName} PRIVATE -Wl,-melf_i386)