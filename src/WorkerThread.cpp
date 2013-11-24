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

#include <memory>
#include <chrono>
using namespace std;

#include "WorkerThread.h"

namespace icke2063 {
namespace common_cpp {

WorkerThread::WorkerThread(shared_ptr<deque<shared_ptr<FunctorInt>>> functor_queue, shared_ptr<MutexInt> functor_lock) :
		WorkerThreadInt(functor_queue, functor_lock), m_running(true) {
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

	m_worker_thread.reset(new std::thread(&WorkerThread::thread_function, this));	// create new scheduler thread											// save pointer of thread object
}

void WorkerThread::stopThread(void){
	m_running = false;
}

WorkerThread::~WorkerThread() {
int i=1;
logger->info("begin ~WorkerThread");
  stopThread();

/*
    while(i++){
	    std::this_thread::sleep_for(std::chrono::microseconds(1));
	    if(m_status == worker_finished)break;
	    if(i>100){
	      logger->emerg("emergency exit: worker thread not joined");
	      return;
	    }
	}
*/
	//at this point the worker thread should have ended -> join it
	if(m_worker_thread.get() && m_worker_thread->joinable()){
		m_worker_thread->join();
	}
	
	logger->info("leave ~WorkerThread");

}

void WorkerThread::thread_function(void) {

	while (m_running) {
	  
	  {
	    	shared_ptr<FunctorInt>curFunctor;
		//if(m_worker_thread.get())m_worker_thread->yield();
		if(p_functor_lock != NULL && ((Mutex*)(p_functor_lock.get()))->getMutex() != NULL){// get new functor from queue
		  
		      shared_ptr<std::mutex> tmp_mutex = ((Mutex*)(p_functor_lock.get()))->getMutex(); 
			std::lock_guard<std::mutex> lock(*tmp_mutex.get());// lock before queue access
			if(!m_running)return;	//running mode changed -> exit thread
			  
			if (p_functor_queue != NULL && p_functor_queue->size() > 0) {
				curFunctor = p_functor_queue->front(); // get next functor from queue
				p_functor_queue->pop_front();				// remove functor from queue
			}
		} else {
			logger->error("p_functor_lock == NULL");
			m_status = worker_idle;
			return;
		}
		if(curFunctor.get() != NULL){
			//logger->debug("get next functor");
			m_status = worker_running;		//
			curFunctor->functor_function(); // call handling function
		} else {
			m_status = worker_idle;			//
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	  }
	}
	m_status = worker_finished;
	logger->debug("exit thread");
}

} /* namespace common_cpp */
} /* namespace icke2063 */
