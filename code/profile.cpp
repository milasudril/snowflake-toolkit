#ifdef __WAND__
target[name[profile.o] type[object] platform[;GNU/Linux]]
#endif

#ifdef PROFILE
#include "profile.h"
#include <sys/time.h>

using namespace SnowflakeModel;

FileOut TicToc::s_stats("./profile_data.txt");


TicToc::TicToc(const char* file,size_t line):r_file(file),m_line(line)
	{
	timeval tv;
	gettimeofday(&tv,nullptr);

	m_start=tv.tv_sec + tv.tv_usec*1e-6;
	}

TicToc::~TicToc()
	{
	timeval tv;
	gettimeofday(&tv,nullptr);
	double now=tv.tv_sec + tv.tv_usec*1e-6;

	s_stats.printf("%s\t%zu\t%.15g\n",r_file,m_line,now-m_start);
	}


#endif
