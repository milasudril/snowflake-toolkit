//@ {
//@	"targets":
//@		[{
//@		 "name":"taskspawntest","type":"application"
//@		,"description":"Application for measuring the overhead of spawning a thread, and wait for it to finish"
//@		}]
//@	}

#include "thread.h"
#include "task.h"
#include <vector>
#include <cstdio>
#include <ctime>

class Dummy:public SnowflakeModel::Task
	{
	public:
		void run() noexcept
			{}
	};

int main()
	{
	try
		{
		auto N_threads_max=SnowflakeModel::Thread::threadsMax();
		for(uint32_t k=0;k<=N_threads_max;++k)
			{
			size_t iter_count=0;
			auto t_0=clock();
			while(clock() - t_0 < CLOCKS_PER_SEC*60)
				{
				std::vector<Dummy> tasks;
				for(uint32_t l=0;l<k;++l)
					{tasks.push_back(Dummy{});}

				std::vector<SnowflakeModel::Thread> threads;
				for(uint32_t l=0;l<k;++l)
					{threads.push_back(SnowflakeModel::Thread{tasks[l],l});}
				++iter_count;
				}
			printf("%u\t%zu\t%.7g\n",k,iter_count,static_cast<double>(clock() - t_0)/CLOCKS_PER_SEC);
			}
		}
	catch(const char* msg)
		{fprintf(stderr,"%s\n",msg);}

	return 0;
	}