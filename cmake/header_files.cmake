function(target_include_kernel_header_files Target)
	target_include_directories(${Target} PRIVATE ${Kernel_INCLUDE_CODE_DIR})
endfunction()