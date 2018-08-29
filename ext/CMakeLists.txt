if(C++11 AND USE_C++11_SUPPORT)

else()
SET(ICKE2063_THREADPOOL_NO_CPP11 1)
endif()

option(${PROJECT_NAME}_NO_DELAYED_TP_SUPPORT "remove support for delayed threadpool" OFF)
option(${PROJECT_NAME}_NO_DYNAMIC_TP_SUPPORT "remove support for dynamic threadpool" OFF)
option(${PROJECT_NAME}_NO_PRIORITY_TP_SUPPORT "remove support for priority threadpool" OFF)

option(ICKE2063_TRHEADPOOL_LOG_SUPPORT "logging support for threadpool" ON)
option(ICKE2063_WORKERTHREAD_LOG_SUPPORT "logging support for workerthreads" ON)
option(ICKE2063_DUMMY_LOG_SUPPORT "logging support for dummy" ON)

if(${PROJECT_NAME}_NO_DELAYED_TP_SUPPORT)
    SET(NO_DELAYED_TP_SUPPORT 1)
endif()	

if(${PROJECT_NAME}_NO_DYNAMIC_TP_SUPPORT)
    SET(NO_DYNAMIC_TP_SUPPORT 1)
endif()	

if(${PROJECT_NAME}_NO_PRIORITY_TP_SUPPORT)
    SET(NO_PRIORITY_TP_SUPPORT 1)
endif()


SET(LOGGING_PREFIX threadpool)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/icke2063_TP_config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/ThreadPool/config/icke2063_TP_config.h
)

configure_file(
	${PROJECT_SOURCE_DIR}/include/logging_macros.h.in
	${CMAKE_CURRENT_BINARY_DIR}/ThreadPool/include/${LOGGING_PREFIX}_logging_macros.h
)

configure_file(
	${PROJECT_SOURCE_DIR}/include/logging_macros.c.in
	${CMAKE_CURRENT_BINARY_DIR}/ThreadPool/include/${LOGGING_PREFIX}_logging_macros.c
)

set(POOL_INC
	"${CMAKE_CURRENT_BINARY_DIR}/ThreadPool/config/"
	"${CMAKE_CURRENT_BINARY_DIR}/ThreadPool/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/include/ThreadPoolInt"
	)

set(EXT_INC
	"${EXT_INC}"
	"${POOL_INC}"
	PARENT_SCOPE
)

set(POOL_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/src/ThreadPool.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/src/WorkerThread.cpp"
	"${CMAKE_CURRENT_BINARY_DIR}/ThreadPool/include/${LOGGING_PREFIX}_logging_macros.c"
	)


set(EXT_SRC
	"${EXT_SRC}"
    "${POOL_SRC}"
	PARENT_SCOPE
	)

### ADD TARGET ###
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${POOL_INC}
	)
	
add_executable(pool_test 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/unittest/pool_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/unittest/TestPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/unittest/DummyFunctor.cpp
    ${POOL_SRC}
    )
UNSET(LOGGING_PREFIX)


if(C++11)
  #enable c++11
  enable_cxx11()
  target_link_libraries(pool_test pthread)
else()
    message(FATAL_ERROR "No C++11 support")
endif()

