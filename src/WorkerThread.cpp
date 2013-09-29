/**
 * @file   WorkerThread.cpp
 * @Author icke2063
 * @date   31.05.2013
 * @brief  WorkerThreadInt implementation
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

#include "WorkerThread.h"
#include <boost/thread.hpp>

namespace icke2063 {
namespace common_cpp {

WorkerThread::WorkerThread(deque<FunctorInt *> *functor_queue, MutexInt *functor_lock) :
		WorkerThreadInt(functor_queue, functor_lock), m_running(true), curFunctor(
				NULL) {
	/**
	 * Init Logging
	 * - set category name
	 * - connect with console
	 * - set loglevel
	 * @todo do this by configfile
	 */
	logger = &log4cpp::Category::getInstance(std::string("WorkerThread"));
	logger->setPriority(log4cpp::Priority::DEBUG);
	if(console)logger->addAppender(console);

	logger->info("WorkerThread");

	m_worker_thread.reset(new boost::thread(&WorkerThread::thread_function, this));	// create new scheduler thread											// save pointer of thread object
}

void WorkerThread::stopThread(void){
	m_running = false;
}

WorkerThread::~WorkerThread() {
	stopThread();
	if(m_worker_thread.get() && m_worker_thread->joinable()){
		m_worker_thread->join();
	}
	while(m_status != worker_finished){
		usleep(1);
	}
	logger->info("~WorkerThread");

}

void WorkerThread::thread_function(void) {

	while (m_running) {
		if(m_worker_thread.get())m_worker_thread->yield();
		if(p_functor_lock != NULL && ((Mutex*)(p_functor_lock))->getMutex() != NULL){
			boost::lock_guard<boost::mutex> lock(*((Mutex*)(p_functor_lock))->getMutex()); // lock before queue access

			if (p_functor_queue != NULL && p_functor_queue->size() > 0) {
				curFunctor.reset(p_functor_queue->front()); // get next functor from queue
				p_functor_queue->pop_front();				// remove functor from queue
			}
		} else {
			logger->error("p_functor_lock == NULL");
			m_status = worker_idle;
			return;
		}

		if(curFunctor.get() != NULL){
			//logger->debug("get next functor");
			m_status = worker_running;
			curFunctor->functor_function(); // call handling function
			if(curFunctor->delete_after_run())
				curFunctor.reset(NULL); 		// reset pointer
			else
				curFunctor.release();
		} else {
			m_status = worker_idle;
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	m_status = worker_finished;
	logger->debug("exit thread");
}

} /* namespace common_cpp */
} /* namespace icke2063 */
