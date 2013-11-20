/**
 * @file   ThreadPool.cpp
 * @Author icke
 * @date   28.05.2013
 * @brief  MBThreadPool implementation
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

//generic
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <string>

#include <chrono>
#include <thread>

//common_cpp
#include <ThreadPool.h>
#include <WorkerThread.h>

namespace icke2063 {
namespace common_cpp {

ThreadPool::ThreadPool() :
		running(true) {
	/**
	 * Init Logging
	 * - set category name
	 * - connect with console
	 * - set loglevel
	 * @todo do this by configfile
	 */
	logger = &log4cpp::Category::getInstance(std::string("ThreadPool"));
	logger->setPriority(log4cpp::Priority::DEBUG);
	if (console)
		//logger->addAppender(console);

	logger->info("ThreadPool");
	m_functor_queue = shared_ptr<deque<shared_ptr<FunctorInt>>>(new deque<shared_ptr<FunctorInt>>); //init functor list	;
	m_functor_lock = shared_ptr<Mutex>(new Mutex()); //init mutex for functor list
	
	m_scheduler_thread.reset(new std::thread(&ThreadPool::scheduler, this)); // create new scheduler thread

}

ThreadPool::~ThreadPool() {
	logger->info("enter ~ThreadPool");
	running = false;	//disable ThreadPool
	if (m_scheduler_thread.get() && m_scheduler_thread->joinable())
		m_scheduler_thread->join();


	//disable/delete Workerthreads
	//disable/delete queued Functors

	logger->info("leave ~ThreadPool");
}

void ThreadPool::pre(void){

	logger->info("ThreadPool scheduler thread");

	//autocreate default count of worker threads
	while (m_workerThreads.size() < this->getLowWatermark()) {
		logger->debug("autocreate new worker thread: %i of %i",
				m_workerThreads.size() + 1, this->getHighWatermark());
		logger->debug("m_functor_lock.get()[%x] m_functor_queue[%x]", m_functor_lock.get(),
				&m_functor_queue);
		WorkerThread *newWorker = new WorkerThread(m_functor_queue, m_functor_lock);
		m_workerThreads.insert(newWorker);
	}

  
}

void ThreadPool::loop(void){
	unsigned int max_func_size = 1;
		// check functor list
		if (m_functor_lock.get() != NULL
				&& ((Mutex*) (m_functor_lock.get()))->getMutex() != NULL) {
			std::lock_guard<std::mutex> lock(
					*((Mutex*) (m_functor_lock.get()))->getMutex()); // lock before queue access

			logger->debug("m_functor_queue.size(): %d", m_functor_queue->size());
			logger->debug("m_workerThreads.size(): %d", m_workerThreads.size());
			logger->debug("max_func_size: %d", max_func_size);

			if (m_functor_queue->size() > max_func_size
					&& m_workerThreads.size() < this->getHighWatermark()) {
				//add new worker thread
				WorkerThread *newWorker = new WorkerThread(m_functor_queue, m_functor_lock);
				logger->debug("create new worker thread[0x%x] ondemand: %i of %i",newWorker,
										m_workerThreads.size() + 1, this->getHighWatermark());
				m_workerThreads.insert(newWorker);
				max_func_size *= 2;
			}

			if (m_functor_queue->size() == 0
					&& m_workerThreads.size() > this->getLowWatermark()) {
				set<WorkerThreadInt *>::iterator workerThreads_it =
						m_workerThreads.begin();
				while (workerThreads_it != m_workerThreads.end()) {
					if ((*workerThreads_it)->m_status == worker_idle) {
						WorkerThread *tmp = (WorkerThread*) (*workerThreads_it);
						logger->debug("finish worker thread[0x%x]: (%i of %i)",tmp,
								m_workerThreads.size(),
								this->getHighWatermark());
						tmp->stopThread();
						max_func_size /= 2;
						if (max_func_size == 0)
							max_func_size = 1;
						break;
					}
					++workerThreads_it;
				}
			}

		} else {
			logger->error("m_functor_lock.get() == NULL");
		}

		{//destroy finished worker
			set<WorkerThreadInt *>::iterator workerThreads_it =
					m_workerThreads.begin();
			while (workerThreads_it != m_workerThreads.end()) {
				if ((*workerThreads_it)->m_status == worker_finished) {
					WorkerThread *tmp = (WorkerThread*) (*workerThreads_it);
					logger->debug("delete worker thread[0x%x]: (%i of %i)",tmp,
							m_workerThreads.size(), this->getHighWatermark());

					delete(tmp);
					m_workerThreads.erase(workerThreads_it);
					workerThreads_it = m_workerThreads.begin();
					continue;
				}
				++workerThreads_it;
			}
		}
		
  
}

void ThreadPool::past(void){

  
}


void ThreadPool::scheduler(void) {

    pre();
    while (running) {
		//m_scheduler_thread->yield();
      loop();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    past();

}

void ThreadPool::addFunctor(shared_ptr<FunctorInt> work) {
	logger->debug("add Functor #%i", m_functor_queue->size() + 1);
	Mutex *tmp = (Mutex *)m_functor_lock.get();
	std::lock_guard<std::mutex> lock(*tmp->getMutex());
	m_functor_queue->push_back(work);
}

DelayedThreadPool::DelayedThreadPool(){
    ///list of delayed functors
    m_delayed_queue =  shared_ptr<deque<shared_ptr<DelayedFunctorInt>>>(new deque<shared_ptr<DelayedFunctorInt>>);

    //lock functor queue
    m_delayed_lock = shared_ptr<MutexInt>(new Mutex());	
    
}


void DelayedThreadPool::loop(void){
	  //std::mutex *mut = (Mutex *)m_delayed_lock.get()
	  
	  lock_guard<std::mutex> lock( *(((Mutex*)(m_delayed_lock.get()))->getMutex().get()) );		//lock 
	  
	  struct timeval timediff, tnow;
	  if( gettimeofday(&tnow, 0) != 0){
	     ThreadPool::loop();	//call scheduler from superclass
	     return;
	  }
	    long msec;
	  
	  /**
	   * @todo use foreach loop
	   */
	  deque<shared_ptr<DelayedFunctorInt>>::iterator delayed_it = m_delayed_queue->begin();
	  while(delayed_it != m_delayed_queue->end()){
	      
	    msec=(tnow.tv_sec-(*delayed_it)->getDeadline().tv_sec)*1000;
	    msec+=(tnow.tv_usec-(*delayed_it)->getDeadline().tv_usec)/1000;	    
	    
	    if(msec >= 0){
	      /*
	      * add current functor to queue & remove from delayed queue
	      */
	      addFunctor((*delayed_it)->getFunctor());
	      delayed_it = m_delayed_queue->erase(delayed_it);
	      continue;
	    }
	    ++delayed_it;
	  }
	  ThreadPool::loop();	//call scheduler from superclass
	  
	}

void DelayedThreadPool::addDelayedFunctor(shared_ptr<FunctorInt> work, struct timeval deadline){
	logger->debug("add DelayedFunctor #%i", m_delayed_queue->size() + 1);
	Mutex *tmp = (Mutex *)m_delayed_lock.get();
	std::lock_guard<std::mutex> lock(*tmp->getMutex());
	shared_ptr<DelayedFunctorInt> tmp_functor = shared_ptr<DelayedFunctorInt>(new DelayedFunctorInt(work, deadline));
	
	m_delayed_queue->push_back(tmp_functor);
}

} /* namespace common_cpp */
} /* namespace icke2063 */
