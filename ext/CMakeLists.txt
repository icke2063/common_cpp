
set(EXT_INC
	${EXT_INC}
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/include/
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/include/ThreadPoolInt
	PARENT_SCOPE
)

set(EXT_SRC
	${EXT_SRC}
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/src/ThreadPool.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool/src/WorkerThread.cpp
	PARENT_SCOPE
	)
