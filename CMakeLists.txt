cmake_minimum_required (VERSION 2.6)
project (common_cpp)

#enable c++11
ADD_DEFINITIONS(
    -std=c++0x # Or -std=c++11
#    # Other flags
)

#default includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCE
	src/Logger.cpp
	src/ThreadPool.cpp
	src/WorkerThread.cpp
	)

add_library(${PROJECT_NAME}-shared SHARED  ${SOURCE})

install(TARGETS ${PROJECT_NAME}-shared
	LIBRARY DESTINATION lib
)