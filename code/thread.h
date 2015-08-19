#ifdef __WAND__
target[name[thread.h] type[include]]
dependency[thread.o]
#endif

#ifndef SNOWFLAKEMODEL_THREAD_H
#define SNOWFLAKEMODEL_THREAD_H

#include <cstdint>
#include <utility>

namespace SnowflakeModel
	{
	class Task;
	class Thread
		{
		public:
			Thread(const Thread&)=delete;
			Thread& operator=(const Thread&)=delete;

			Thread& operator=(Thread&& obj) noexcept
				{
				std::swap(pimpl,obj.pimpl);
				return *this;
				}

			Thread(Thread&& obj) noexcept
				{
				pimpl=obj.pimpl;
				obj.pimpl=nullptr;
				}

			explicit Thread(Task& task, uint32_t thread_count);

			static uint32_t threadsMax();

			~Thread();

		private:
			struct Impl;
			Impl* pimpl;

		};
	}

#endif
