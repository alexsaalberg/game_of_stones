cmake_minimum_required(VERSION 2.7)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

macro(print_var var_name)
	message(STATUS "${var_name}=${${var_name}}")
endmacro()

function(prepend_filepath files_var_name prefix)
   	set(prepended_files "")
	set(input_files ${${files_var_name}})
    foreach(file ${input_files})
    	list(APPEND prepended_files "${prefix}/${file}")
   	endforeach(file)
    set(${files_var_name} "${prepended_files}" PARENT_SCOPE)
endfunction(prepend_filepath)
