//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"profile.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"profile.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_PROFILE_H
#define SNOWFLAKEMODEL_PROFILE_H

#ifdef PROFILE

#include "file_out.h"

namespace SnowflakeModel
	{
	class TicToc
		{
		public:
			TicToc(const char* file,size_t line);
			~TicToc();

		private:
			static FileOut s_stats;
			const char* r_file;
			size_t m_line;
			double m_start;
		};
	}

#define SNOWFLAKEMODEL_TIMED_SCOPE() SnowflakeModel::TicToc __profiler_object\
	{__FILE__,__LINE__}

#else
	#define  SNOWFLAKEMODEL_TIMED_SCOPE()
#endif

#endif
