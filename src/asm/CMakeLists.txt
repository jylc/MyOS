project(asm ASM)

# ÉèÖÃ.sÎÄ¼ş
message(STATUS "Project:${PROJECT_NAME}")
find_asm_source_file(asm_file_src ${asm_SOURCE_DIR})
set(asm_src ${asm_file_src})
add_library(${PROJECT_NAME} OBJECT ${asm_src})