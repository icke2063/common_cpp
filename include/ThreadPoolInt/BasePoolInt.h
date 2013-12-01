/**
 * @file   BasePoolInt.h
 * @Author icke2063
 * @date   26.05.2013
 *
 * @brief	Framework for a "Threadpool". Therefore within this file the ThreadPool, WorkerThread and Functor
 * 		classes are defined. The Threadpool shall be used to handle Functor
 * 		objects. These functors are handled by WorkerThreads.
 * 		The inherit class has to define the abstract functions with own or external thread and mutex
 * 		implementations (e.g. c++11, boost...).
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

#ifndef BASEPOOLINT_H_
#define BASEPOOLINT_H_

//std libs
#include <stdint.h>
#include <deque>
#include <list>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
  #include <memory>
  #include <chrono>
  #include <thread>
  #include <mutex>
  using namespace std;
#else
  //Boost fallback
  #include <boost/shared_ptr.hpp>
  #include <boost/thread.hpp>
  using namespace boost;
  
  #define unique_ptr scoped_ptr
#endif

namespace icke2063 {
namespace common_cpp {

///Functor for ThreadPool
/**
 * Inherit from this class then it can be added by ThreadPool::addFunctor with an implementation
 * of the ThreadPool class. This class has to implement the functor_function. All member functions and variables
 * are accessable by the functor_function. This function will be executed by the WorkerThreads
 *
 * @todo make sure that it cannot declared as static variable
 */
class FunctorInt {  
public:
	FunctorInt(){};
	virtual ~FunctorInt(){};
	/**
	 * Function called by WorkerThread
	 * @brief This function will be called by WorkerThreadInt of ThreadPool
	 */
	virtual void functor_function(void) = 0;
};

///WorkerThread of ThreadPool
/**
 * The inherit objects of this class are used to handle new functors from functor_queue
 * and call their functor function.
 * The implementation has to create for each object of this class an own thread.
 */

/**
 * enumeration of WorkerThread states
 */
enum worker_status{
	worker_idle=0x00,   	//!< worker_idle
	worker_running=0x01,	//!< worker_running
	worker_finished=0x02	//!< worker_finished
};

class WorkerThreadInt {
public:
	/**
	 * Standard Constructor
	 * @param functor_queue: reference threadpool functor queue
	 * @param functor_lock:  reference to threadpool mutex object (type depend on implementation)
	 */
	WorkerThreadInt(shared_ptr<std::deque<shared_ptr<FunctorInt> > > functor_queue, shared_ptr<mutex> functor_lock):
		m_worker_running(true),
		p_functor_queue(functor_queue),p_functor_lock(functor_lock),m_status(worker_idle){
		// create new worker thread
		m_worker_thread.reset(new thread(&WorkerThreadInt::thread_function, this));
		
	};
	virtual ~WorkerThreadInt(){
	  m_worker_running = false;	//disable worker threads
	  //at this point the worker thread should have ended -> join it
	  if(m_worker_thread.get() && m_worker_thread->joinable()){
		m_worker_thread->join();
	  }
	  printf("~WorkerThread\n");
	};

	/**
	 * Status of current WorkerThread
	 * This Threadpool has the ability to create/destroy WorkerThread objects.
	 * The current solution is to set a status value at each worker to let
	 * the scheduler decide which worker can be destroyed.
	 */
	worker_status m_status;					//status of current thread

protected:
	shared_ptr<std::deque<shared_ptr<FunctorInt> > > p_functor_queue;	//reference to queue of ThreadPool functor list
	shared_ptr<mutex> p_functor_lock;				//reference to lock for ThreadPool functor list

virtual	void thread_function(void) {
    while (m_worker_running) {
      {
	    shared_ptr<FunctorInt>curFunctor;
	    this_thread::yield();
	    if(p_functor_lock.get()){// got correct functor  queue?
	      
		  lock_guard<mutex> lock(*p_functor_lock.get());// lock before queue access
		  
		    if(!m_worker_running)return;	//running mode changed -> exit thread
		    
		    if (p_functor_queue.get() != NULL && p_functor_queue->size() > 0) {
			    curFunctor = p_functor_queue->front(); 			// get next functor from queue
			    p_functor_queue->pop_front();				// remove functor from queue			  
		    }
	    } else {
		    m_status = worker_finished;
		    return;
	    }
	    if(curFunctor.get() != NULL){
		    //logger->debug("get next functor");
		    m_status = worker_running;		//
		    curFunctor->functor_function(); // call handling function
	    } else {
		    m_status = worker_idle;			//
		    this_thread::sleep_for(chrono::microseconds(10));
	    }
      }
    }
    m_status = worker_finished;
}
		
private:
  	/**
	 * worker thread object
	 */
	unique_ptr<thread> m_worker_thread;
	/**
	 * running flag for worker thread
	 *
	 */
	bool m_worker_running;
};
///Abstract ThreadPool interface
class BasePoolInt {
  /**
   * addFunctor modes
   * 
   */
#define TPI_ADD_Default	0
#define TPI_ADD_FiFo	1
#define TPI_ADD_LiFo	2  
public:
	BasePoolInt() :m_main_sleep_us(1000),m_main_running(true) {
	/* init all shared objects with new objects */
	m_functor_queue = shared_ptr<std::deque<shared_ptr<FunctorInt> > >(new std::deque<shared_ptr<FunctorInt> >); //init functor list	;
	m_functor_lock = shared_ptr<mutex>(new mutex()); //init mutex for functor list
	m_worker_lock = shared_ptr<mutex>(new mutex()); //init mutex for worker list
	  
	addWorker();	//add at least one worker thread
}
	
virtual ~BasePoolInt(){
m_main_running = false;	//disable ThreadPool

if (m_main_thread.get() && m_main_thread->joinable())
	m_main_thread->join();
}

void startPoolLoop(){
  printf("startPoolLoop\n");
  m_main_thread.reset(new thread(&BasePoolInt::main_thread_func, this)); // create new main thread_function
}

/**
  * 	Add new functor to queue
  * 	this function MUST be implemented by inherit class
  * 	IMPORTANT: don't forget to lock the queue with Mutex (depends on implementation)
  * @param work
  */
virtual bool addFunctor(shared_ptr<FunctorInt> work, uint8_t add_mode = TPI_ADD_Default){
    lock_guard<mutex> lock(*m_functor_lock.get());
    m_functor_queue->push_front(work);
    printf("m_functor_queue->size():%i\n",m_functor_queue->size());
}
/**
 * get current worker count within worker list
 */
uint16_t getWorkerCount(){return m_workerThreads.size();}

/**
 * get current functor size of functor queue
 */
uint16_t getQueueCount(){return m_functor_queue.get()?m_functor_queue->size():0;}
	
virtual bool addWorker( void ){
  if(m_worker_lock.get()){
	    lock_guard<mutex> lock(*m_worker_lock.get()); // lock before worker access
  try{
	    shared_ptr<WorkerThreadInt> newWorker = shared_ptr<WorkerThreadInt>(new WorkerThreadInt(m_functor_queue, m_functor_lock));
	    m_workerThreads.push_back(newWorker);    
  }catch(exception e){
  return false; 
}
  return true;  
  } 
  return false;
}
	
	
protected:
  	/* function called before main thread loop */
	virtual void main_pre(void) = 0;
	/* function called within main thread loop */
	virtual void main_loop(void) = 0;
	/* function called after main thread loop */
	virtual void main_past(void) = 0;
	
	/**
	 * main thread function
	 * - call main_pre and main_past once
	 * - continously calling main_loop while running flag is true
	 */
	virtual void main_thread_func(void){
	    main_pre();
	    while (m_main_running) {
		//m_scheduler_thread->yield();
	      main_loop();
	      this_thread::sleep_for(chrono::microseconds(m_main_sleep_us));
	    }
	    main_past();
	}
		
protected:
	///list of used WorkerThreadInts
	std::list<shared_ptr<WorkerThreadInt> > 		m_workerThreads;

	///lock worker queue
	shared_ptr<mutex> 				m_worker_lock;

	///list of waiting functors
	shared_ptr<std::deque<shared_ptr<FunctorInt> > > 	m_functor_queue;

	///lock functor queue
	shared_ptr<mutex>				m_functor_lock;

	/// sleep time between every loop of main thread 
	uint32_t m_main_sleep_us;
	///running flag
	bool m_main_running;
	
private:
  	/**
	 * Scheduler is used for creating and scheduling the WorkerThreads.
	 * - on high usage (many unhandled functors in queue) create new threads until HighWatermark limit
	 * - on low usage and many created threads -> delete some to save resources
	 */
	unique_ptr<thread> m_main_thread;
};

} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* BASEPOOLINT_H_ */
