cmake_minimum_required (VERSION 2.6)
project (common_cpp)

#default includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCE
	src/Logger.cpp
	src/ThreadPool.cpp
	src/WorkerThread.cpp
	)

add_library(${PROJECT_NAME} SHARED  ${SOURCE})