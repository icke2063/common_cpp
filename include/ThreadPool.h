/**
 * @file   ThreadPool.h
 * @Author icke2063
 * @date   28.05.2013
 * @brief  ThreadPoolInt implementation with usage of boost threads, mutex,...
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

#include <auto_ptr.h>
using namespace std;

//common_cpp
#include <ThreadPoolInt.h>
#include <Logger.h>

//boost
#include <boost/thread.hpp>

namespace icke2063 {
namespace common_cpp {

class Mutex: public MutexInt {
public:
	Mutex(){m_mutex.reset(new boost::mutex());};
	virtual ~Mutex(){};

	/**
	 * Getter for boost mutex object pointer
	 * @return pointer to boost mutex object
	 */
	virtual boost::mutex *getMutex(void){return m_mutex.get();}

private:
	/**
	 * boost mutex object
	 */
	auto_ptr<boost::mutex> m_mutex;
};


class ThreadPool: public ThreadPoolInt ,public Logger{
public:
	ThreadPool();
	virtual ~ThreadPool();
	/**
	 * Default scheduler thread function
	 * - create/destroy WorkerThreads on demand
	 */
	virtual void scheduler(void);

	/**
	 * Add new functor object
	 * @param work pointer to functor object
	 */
	virtual void addFunctor(FunctorInt *work);

private:
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
	auto_ptr<boost::thread> m_scheduler_thread;
};

} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* THREADPOOL_H_ */
