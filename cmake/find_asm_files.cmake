# ��DirctoryĿ¼��Ѱ��.s�ļ�
macro(find_asm_source_file OutValue Directory)
	file(GLOB ${OutValue} LIST_DIRECTORIES false ${Directory}/*.[sS])
endmacro()