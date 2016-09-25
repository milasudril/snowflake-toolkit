//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[
//@	                {
//@	                    "ref":"pthread",
//@	                    "rel":"external"
//@	                }
//@	            ],
//@	            "name":"thread.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "thread.h"
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cassert>

using namespace SnowflakeModel;

namespace
	{
	void* thread_startup(void* threadcallback)
		{
		assert(threadcallback!=nullptr);
		( *reinterpret_cast<ThreadBase::Callback*>(threadcallback) )();
		return nullptr;
		}
	}

uint32_t ThreadBase::threadsMax()
	{
	return sysconf(_SC_NPROCESSORS_ONLN);
	}



ThreadBase::ThreadBase(ThreadBase::Callback& cb,uint32_t thread_count)
	{
	pthread_t handle;
	int res;
	do
		{res=pthread_create(static_cast<pthread_t*>(&handle),nullptr,thread_startup,&cb);}
	while(res==EAGAIN); //It may happen that the system has no time to wait for remaining threads
	
	if(res!=0)
		{
		fprintf(stderr,"Errno: %d\n",res);
		throw "Could not create a new thread";
		}

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset) ;
	CPU_SET(thread_count,&cpuset);
	pthread_setaffinity_np(handle,sizeof(cpuset),&cpuset);
	
	static_assert(sizeof(m_handle)>=sizeof(handle),"Bad handle type");
	m_handle=static_cast<intptr_t>(handle);
	m_dead=0;
	}

ThreadBase::~ThreadBase()
	{
	if(m_dead)
		{return;}
	auto handle=static_cast<pthread_t>(m_handle);
	void* result=nullptr;
	if(pthread_join(handle,&result)!=0)
		{abort();}
	}

void ThreadBase::wait() const
	{
	if(m_dead)
		{return;}
	auto handle=static_cast<pthread_t>(m_handle);
	void* result=nullptr;
	if(pthread_join(handle,&result)!=0)
		{throw "It was not possible to synchronize threads";}
	m_dead=1;
	}
