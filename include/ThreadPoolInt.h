/**
 * @file   ThreadPoolInt.h
 * @Author icke2063
 * @date   26.05.2013
 *
 * @brief  Framework for a "Threadpool". Therefore within this file the ThreadPool, WorkerThread and Functor
 * 		   classes are defined. The Threadpool shall be used to handle Functor
 * 		   objects. These functors are handled by WorkerThreads.
 * 		   The inherit class has to define the abstract functions with own or external thread
 * 		   implementations (e.g. boost).
 *
 * Copyright © 2013 icke2063 <icke2063@gmail.com>
 *
 * This framework is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */



#ifndef THREADPOOLINT_H_
#define THREADPOOLINT_H_

#ifndef WORKERTHREAD_MAX
	#define WORKERTHREAD_MAX	40
#endif

//std libs
#include <stdint.h>
#include <deque>
#include <set>
#include <auto_ptr.h>
using namespace std;

namespace icke2063 {
namespace common_cpp {

class MutexInt {
public:
	MutexInt(){};
	virtual ~MutexInt(){};
};

///Functor for ThreadPool
/**
 * Inherit from this class then it can be added by ThreadPool::addFunctor with an implementation
 * of the ThreadPool class. This class has to implement the functor_function. All member functions and variables
 * are accessable by the functor_function. This function will be executed by the WorkerThreadInts
 * and after execution this object will be deleted. Make sure that this object is dynamically created and is
 * not referenced after adding to ThreadPool.
 *
 * @todo make sure that it cannot declared as static variable
 */
class FunctorInt {
public:
	FunctorInt(bool del=true):m_del(del){};
	virtual ~FunctorInt(){};
	/**
	 * Function called by WorkerThread
	 * @brief This function will be called by WorkerThreadInt of ThreadPool
	 */
	virtual void functor_function(void) = 0;

	bool delete_after_run(void){return m_del;}

private:
	/**
	 * flag to force deletion by workerthread after run
	 */
	bool m_del;
};

///WorkerThread of ThreadPool
/**
 * The inherit objects of this class are used to get new Functors from functor_queue
 * and call their functor function.
 * The implementation has to create for each object of this class an own thread.
 */

enum worker_status{
	worker_idle=0x00,
	worker_running=0x01,
	worker_finished=0x02
};
class WorkerThreadInt {
public:
	WorkerThreadInt(deque<FunctorInt *> *functor_queue, MutexInt *functor_lock):
		p_functor_queue(functor_queue),p_functor_lock(functor_lock),m_status(worker_idle){};
	virtual ~WorkerThreadInt(){};
	worker_status m_status;					//status of current thread

protected:
	deque<FunctorInt *> *p_functor_queue;	//pointer to queue of ThreadPool functor list
	MutexInt *p_functor_lock;				//pointer to lock for ThreadPool functor list

	/**
	 * Abstract function which has to be implemented and used with
	 * thread implementation. It has to get the next functor from functor queue and
	 * call the functor_function.
	 * After execution finished -> delete functor object
	 */
	virtual void thread_function (void) = 0;

};
///Abstract ThreadPool interface
class ThreadPoolInt {
public:
	ThreadPoolInt() :
			HighWatermark(1), LowWatermark(1) {
	}
	;
	virtual ~ThreadPoolInt(){};

	/**
	 * set low watermark
	 * @param low: low count of WorkerThreadInts
	 */
	void setLowWatermark(uint8_t low) {
		LowWatermark =
				((low < HighWatermark)) ? low : HighWatermark;
	}
	/**
	 * get low count of WorkerThreads
	 * @return lowWatermark
	 */
	uint8_t getLowWatermark(void){return LowWatermark;}

	/**
	 * set high watermark
	 * @param high: high count of WorkerThreadInts
	 */
	void setHighWatermark(uint8_t high){
		HighWatermark =
				((high > LowWatermark) && (high < WORKERTHREAD_MAX)) ? high : WORKERTHREAD_MAX;
	}

	/**
	 * Get high count of WorkerThreads
	 * @return highWatermark
	 */
	uint8_t getHighWatermark(void){return HighWatermark;}

	/**
	 * 	This function is used to create needed WorkerThreadInts
	 * 	and to destroy (really) not needed WorkerThreadInts.
	 * 	MUST be implemented by inherit class
	 */
	virtual void scheduler(void) = 0;

	/**
	 * 	Add new functor to queue
	 * 	this function MUST be implemented by inherit class
	 * 	IMPORTANT: don't forget to lock the queue with Mutex (depends on implementation)
	 * @param work
	 */
	virtual void addFunctor(FunctorInt *work) = 0;

protected:
	///list of used WorkerThreadInts
	set<WorkerThreadInt *> 	m_workerThreads;

	///lock worker queue
	auto_ptr<MutexInt> 				m_worker_lock;

	///list of waiting functors
	deque<FunctorInt *> 		m_functor_queue;

	///lock functor queue
	auto_ptr<MutexInt>				m_functor_lock;

private:
	uint8_t LowWatermark;	//low count of worker threads
	uint8_t HighWatermark;	//high count of worker threads
};

} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* THREADPOOLINT_H_ */
