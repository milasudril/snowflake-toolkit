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
#include "task.h"
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

using namespace SnowflakeModel;

namespace
	{
	void* thread_startup(void* task)
		{
		((Task*)task)->run();
		return nullptr;
		}
	}

uint32_t Thread::threadsMax()
	{
	return sysconf(_SC_NPROCESSORS_ONLN);
	}

struct Thread::Impl
	{
	Impl(Task& task,uint32_t thread_count)
		{
		if(pthread_create(&handle,nullptr,thread_startup,&task)!=0)
			{throw "Could not create a new thread";}

		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(thread_count, &cpuset);

		pthread_setaffinity_np(handle, sizeof(cpu_set_t), &cpuset);
		}

	~Impl()
		{
		void* result=nullptr;
		if(pthread_join(handle,&result)!=0)
			{abort();}
		}

	pthread_t handle;
	};

Thread::Thread(Task& task,uint32_t thread_count)
	{
	pimpl=new Impl(task,thread_count);
	}

Thread::~Thread()
	{
	delete pimpl;
	}
