cmake_minimum_required (VERSION 2.6)
project (common_cpp)

option(USE_C++11_SUPPORT "use c++11 if available" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CXX11-CMake-Macros")

include(CXX11)

check_for_cxx11_compiler(C++11)

if(C++11 AND USE_C++11_SUPPORT)
  #enable c++11
  enable_cxx11()
else()
  FIND_PACKAGE(Boost COMPONENTS thread system serialization chrono REQUIRED)
endif()

add_subdirectory(ext)

#message(FATAL_ERROR ${EXT_INC})

#default includes

set(INC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${EXT_INC}"
	)

set(${PROJECT_NAME}_INC
    "${INC}"
    PARENT_SCOPE
	)

include_directories(
    ${INC}
)

set(${PROJECT_NAME}_SRC
	${EXT_SRC}
	)
		
add_library(${PROJECT_NAME}-shared SHARED  ${${PROJECT_NAME}_SRC})

target_link_libraries(${PROJECT_NAME}-shared
	${Boost_LIBRARIES}
  )

install(TARGETS ${PROJECT_NAME}-shared
	LIBRARY DESTINATION lib
)