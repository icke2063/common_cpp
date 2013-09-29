/**
 * @file   WorkerThread.h
 * @Author icke2063
 * @date   31.05.2013
 * @brief  WorkerThreadInt implementation with boost threads
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

#ifndef WORKERTHREAD_H_
#define WORKERTHREAD_H_

//generic
#include <auto_ptr.h>
using namespace std;

//boost
#include <boost/thread.hpp>

//common_cpp
#include <ThreadPool.h>
#include <Logger.h>

namespace icke2063 {
namespace common_cpp {

class WorkerThread: public WorkerThreadInt ,public Logger{
public:
	WorkerThread(deque<FunctorInt *> *functor_queue, MutexInt *functor_lock);

	virtual ~WorkerThread();

	void startThread(void);
	void stopThread(void);

private:
	/**
	 * boost worker thread object
	 */
	auto_ptr<boost::thread> m_worker_thread;
	/**
	 * running flag for worker thread
	 *
	 */
	bool m_running;

	auto_ptr<FunctorInt>curFunctor;

	/**
	 * thread function for worker thread
	 * - loop over functor list(hold from threadpool)
	 * - call functior function
	 * - destroy functor object (if destroy flag is set)
	 */
	virtual void thread_function (void);
};

} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* WORKERTHREAD_H_ */
