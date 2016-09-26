//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"thread.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"thread.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_THREAD_H
#define SNOWFLAKEMODEL_THREAD_H

#include <cstdint>
#include <utility>

namespace SnowflakeModel
	{
	class ThreadBase
		{
		public:
			class Callback
				{
				public:
					virtual void operator()() noexcept=0;
				};

			ThreadBase(const ThreadBase&)=delete;
			ThreadBase& operator=(const ThreadBase&)=delete;

			ThreadBase& operator=(ThreadBase&& obj)
				{
				std::swap(obj.m_handle,m_handle);
				return *this;
				}
		
		/*
			ThreadBase(ThreadBase&& obj):m_handle(obj.m_handle)
				{
				obj.markAsDead();
				markAsLiving();
				}
		*/
			ThreadBase(ThreadBase&& obj)=delete;

			explicit ThreadBase(Callback& task, uint32_t thread_count);

			static uint32_t threadsMax();
			
			~ThreadBase();
			
			void wait() const;

		private:
			intptr_t m_handle;
		#if __unix__ || __linux__
		//	The pthread standard does not give us any way of detecting a dead thread
			mutable bool m_dead;
			void markAsDead()
				{m_dead=1;}
			void markAsLiving()
				{m_dead=0;}
		#else
			void markAsDead(){}
			void markAsLiving(){}
		#endif
		};

	template<class Task>
	class Thread
		{
		private:
			class CallbackImpl:public ThreadBase::Callback
				{
				public:
					explicit CallbackImpl(Task&& t):m_task(std::move(t))
						{}
	
					void operator()() noexcept
						{m_task();}

					const Task& taskGet() const noexcept
						{return m_task;}

				private:
					Task m_task;
				};

		public:
			Thread(Task&& task,uint32_t thread_count):m_cb(std::move(task))
				,m_thread(m_cb,thread_count)
				{}

			const Task& taskGet() const noexcept
				{
				m_thread.wait();
				return m_cb.taskGet();
				}

		private:
			CallbackImpl m_cb;
			ThreadBase m_thread;
		};

	inline uint32_t threadsCountGet() noexcept
		{
		return ThreadBase::threadsMax();
		}
	}

#endif
