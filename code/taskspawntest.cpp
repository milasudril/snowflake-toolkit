//@ {"targets":[{"name":"taskspawntest","type":"application"}]}

#include "thread.h"
#include <vector>
#include <cstdio>
#include <ctime>

class Task
	{
	public:
		void operator()() noexcept
			{}
	};

int main()
	{
	try
		{
		throw "This program is currently broken";
		auto N_threads_max=SnowflakeModel::threadsCountGet();
		for(uint32_t k=0;k<=N_threads_max;++k)
			{
			size_t iter_count=0;
			auto t_0=clock();
			while(clock() - t_0 < CLOCKS_PER_SEC*60)
				{
			//FIXME: std::vector does not work!!!
			/*	std::vector< SnowflakeModel::Thread<Task> > threads;
				threads.reserve(N_threads_max);
					for(uint32_t l=0;l<k;++l)
					{
					threads.push_back(SnowflakeModel::Thread<Task>{Task(),l});
					}
				++iter_count;*/
				}
			printf("%u\t%zu\t%.7g\n",k,iter_count,static_cast<double>(clock() - t_0)/CLOCKS_PER_SEC);
			}
		}
	catch(const char* msg)
		{fprintf(stderr,"%s\n",msg);}

	return 0;
	}