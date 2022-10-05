project(boot ASM)

# ÉèÖÃ.sÎÄ¼þ
message(STATUS "Project:${PROJECT_NAME}")
find_asm_source_file(boot_asm_src ${boot_SOURCE_DIR})
set(boot_src ${boot_asm_src})
add_library(${PROJECT_NAME} OBJECT ${boot_src})