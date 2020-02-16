cmake_minimum_required (VERSION 2.6)
project (common_cpp)

option(common_cpp_I2C_SUPPORT "compile with I2C support" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CXX11-CMake-Macros")
include(CXX11)
check_for_cxx11_compiler(C++11)
if(C++11 AND USE_C++11_SUPPORT)
    #enable c++11
    enable_cxx11()
else()
    message(FATAL_ERROR "No C++11 support")
endif()

add_subdirectory(ext)

#default includes
set(INC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${EXT_INC}"
	)

set(common_cpp_INC
    "${INC}"
    PARENT_SCOPE
	)

include_directories(
    ${INC}
)

set(common_cpp_SRC
    ${EXT_SRC}
	)

if(common_cpp_I2C_SUPPORT)

    # generate i2c logging files
    SET(LOGGING_PREFIX i2c)
    configure_file(
    	include/logging_macros.h.in
    	${CMAKE_CURRENT_BINARY_DIR}/${LOGGING_PREFIX}_logging_macros.h
    )
    
    configure_file(
    	include/logging_macros.c.in
    	${CMAKE_CURRENT_BINARY_DIR}/${LOGGING_PREFIX}_logging_macros.c
    )
        
    set(common_cpp_SRC ${common_cpp_SRC}
        "src/I2CComm.cpp"
        ${CMAKE_CURRENT_BINARY_DIR}/${LOGGING_PREFIX}_logging_macros.c
	)
	
    UNSET(LOGGING_PREFIX)
endif(common_cpp_I2C_SUPPORT)

		
add_library(common_cpp-shared SHARED  
       ${common_cpp_SRC}
    )

install(TARGETS ${PROJECT_NAME}-shared
	LIBRARY DESTINATION lib
)