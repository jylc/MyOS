function(target_include_header_files Target)
	target_include_directories(${Target} PRIVATE ${Kernel_INCLUDE_CODE_DIR})
	
endfunction()


function(target_include_common_header_files Target)
	target_include_directories(${Target} PRIVATE ${Kernel_INCLUDE_CODE_DIR}/common)
endfunction()

function(target_include_tools_header_files Target)
	target_include_directories(${Target} PRIVATE ${Kernel_INCLUDE_CODE_DIR}/tools)
endfunction()

function(target_include_tools_lib_header_files Target)
	target_include_directories(${Target} PRIVATE ${Kernel_INCLUDE_CODE_DIR}/tools/lib)
endfunction()