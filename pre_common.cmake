include_directories(AFTER SYSTEM
	${PROJECT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_BINARY_DIR}/include
)
# Copy the "include" directory into the "build" directory for building. 
function(copy_include_to_build)
	exec_program(mkdir ARGS "-p ${CMAKE_BINARY_DIR}/include")
	exec_program(cp ARGS "-rf ${PROJECT_SOURCE_DIR}/include/ ${CMAKE_BINARY_DIR}/")
endfunction(copy_include_to_build)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)



