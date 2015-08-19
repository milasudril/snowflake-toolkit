#ifdef __WAND__
target[name[solid_writer.h] type[include]]
dependency[solid_writer.o]
#endif

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
