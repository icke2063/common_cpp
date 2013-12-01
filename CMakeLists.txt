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


#default includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/ThreadPoolInt)


set(SOURCE
	src/Logger.cpp
	src/ThreadPool.cpp
	src/WorkerThread.cpp
	)
	
add_library(${PROJECT_NAME}-shared SHARED  ${SOURCE})

target_link_libraries(${PROJECT_NAME}-shared
	${Boost_LIBRARIES}
  )

install(TARGETS ${PROJECT_NAME}-shared
	LIBRARY DESTINATION lib
)