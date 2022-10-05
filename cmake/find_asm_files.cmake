# 从Dirctory目录下寻找.s文件
macro(find_asm_source_file OutValue Directory)
	file(GLOB ${OutValue} LIST_DIRECTORIES false ${Directory}/*.[sS])
endmacro()