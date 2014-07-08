
option(${PROJECT_NAME}_NO_DELAYED_TP_SUPPORT "remove support for delayed threadpool" OFF)
option(${PROJECT_NAME}_NO_DYNAMIC_TP_SUPPORT "remove support for dynamic threadpool" OFF)
option(${PROJECT_NAME}_NO_PRIORITY_TP_SUPPORT "remove support for priority threadpool" OFF)


set(POOL_INC
	"${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/config/"
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
	)

set(EXT_SRC
	"${EXT_SRC}"
    "${POOL_SRC}"
	PARENT_SCOPE
	)

### ADD TARGET ###
include_directories(
    ${POOL_INC}
	)
	
if(${PROJECT_NAME}_NO_DELAYED_TP_SUPPORT)
    add_definitions( "-DNO_DELAYED_TP_SUPPORT=1" )
endif()	
	

if(${PROJECT_NAME}_NO_DYNAMIC_TP_SUPPORT)
    add_definitions( "-DNO_DYNAMIC_TP_SUPPORT=1" )
endif()	

if(${PROJECT_NAME}_NO_PRIORITY_TP_SUPPORT)
    add_definitions( "-DNO_PRIORITY_TP_SUPPORT=1" )
endif()	


add_executable(pool_test 
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/unittest/pool_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/unittest/TestPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/unittest/DummyFunctor.cpp
    ${POOL_SRC}
    )

##TheadPool logging
#add_definitions( "-DThreadPool_log_error=printf" )
#add_definitions( "-DThreadPool_log_info=printf" )
#add_definitions( "-DThreadPool_log_debug=printf" )
#add_definitions( "-DThreadPool_log_trace=printf" )

##worker thread logging
#add_definitions( "-DWorkerThread_log_error=printf" )
#add_definitions( "-DWorkerThread_log_info=printf" )
#add_definitions( "-DWorkerThread_log_debug=printf" )
#add_definitions( "-DWorkerThread_log_trace=printf" )

##dummy functor logging
#add_definitions( "-DDummy_Functor_log_error=printf" )
#add_definitions( "-DDummy_Functor_log_info=printf" )
#add_definitions( "-DDummy_Functor_log_debug=printf" )
#add_definitions( "-DDummy_Functor_log_trace=printf" )




if(C++11 AND USE_C++11_SUPPORT)
  #enable c++11
  enable_cxx11()
  target_link_libraries(pool_test pthread)
else()
    FIND_PACKAGE(Boost COMPONENTS thread system serialization chrono REQUIRED)
    target_link_libraries(pool_test ${Boost_LIBRARIES})
endif()

