//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"solid_writer.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_writer.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_SOLIDWRITER_H
#define SNOWFLAKEMODEL_SOLIDWRITER_H

#include <cstdio>

namespace SnowflakeModel
	{
	class Solid;
	class FileOut;
		
	class SolidWriter
		{
		public:
			SolidWriter(FileOut& dest);
			void write(const Solid& solid);
			
		private:
			FileOut& r_dest;
		};
	}

#endif
