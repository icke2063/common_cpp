/**
 * @file   ThreadPool.h
 * @Author icke2063
 * @date   28.05.2013
 * @brief  ThreadPoolInt implementation with usage of c++11 threads, mutex,...
 *
 * Copyright © 2013 icke2063 <icke2063@gmail.com>
 *
 * This software is free; you can redistribute it and/or
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

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <sys/time.h>

#include <memory>
#include <mutex>
#include <thread>
using namespace std;

//common_cpp
#include <ThreadPoolInt.h>
#include <Logger.h>

namespace icke2063 {
namespace common_cpp {

/**
 * MutexInt implementation with c++11::mutex
 */
class Mutex: public MutexInt {
public:
	Mutex(){
	  m_mutex = shared_ptr<std::mutex>(new std::mutex());
	}
	virtual ~Mutex(){};

	/**
	 * Getter for c++11::mutex object pointer
	 * @return pointer to c++11 mutex object
	 */
	virtual shared_ptr<std::mutex> getMutex(void){return m_mutex;}

private:
	/**
	 * mutex object
	 */
	shared_ptr<std::mutex> m_mutex;
};


class ThreadPool: public ThreadPoolInt ,public Logger{
public:
	ThreadPool();
	virtual ~ThreadPool();

	virtual void pre(void);
	virtual void loop(void);
	virtual void past(void);
	
	/**
	 * Add new functor object
	 * @param work pointer to functor object
	 */
	virtual void addFunctor(shared_ptr<FunctorInt> work);

private:
	/**
	 * Default scheduler thread function
	 * - create/destroy WorkerThreads on demand
	 */
	void scheduler(void);

	/**
	 * running flag for scheduler thread
	 *
	 */
	bool running;

	/**
	 * Scheduler is used for creating and scheduling the WorkerThreads.
	 * - on high usage (many unhandled functors in queue) create new threads until HighWatermark limit
	 * - on low usage and many created threads -> delete some to save resources
	 */
	unique_ptr<std::thread> m_scheduler_thread;
};

 class DelayedFunctorInt{
 public:
   /**
    * default constructor
    * - set deadline
    */
   DelayedFunctorInt(shared_ptr<FunctorInt> functor, struct timeval deadline):
   m_functor(functor),m_deadline(deadline){}
   
   
   /**
    * get functor deadline
    * - after this timestamp functor should be activated
    */
   struct timeval getDeadline(){return m_deadline;}
   
   /**
    * get stored FunctorInt
    */
   shared_ptr<FunctorInt> getFunctor(){return m_functor;}
 
 private:   
   shared_ptr<FunctorInt> m_functor;
   /**
    * abslute timestamp after this deadline the functor should be added to threadpool
    * 
    */
   struct timeval m_deadline;
};

class DelayedThreadPool: public ThreadPool{ 
public:  
	DelayedThreadPool();
	virtual ~DelayedThreadPool(){}

	virtual void loop(void);
	
	/**
	 * Add new functor object
	 * @param work pointer to functor object
	 */
	virtual void addDelayedFunctor(shared_ptr<FunctorInt> work, struct timeval deadline);

private:
  ///list of delayed functors
  shared_ptr<deque<shared_ptr<DelayedFunctorInt>>> 	m_delayed_queue;

  ///lock functor queue
  shared_ptr<MutexInt>					m_delayed_lock;
  
};



} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* THREADPOOL_H_ */
