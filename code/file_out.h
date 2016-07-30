//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"file_out.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_FILEOUT_H
#define SNOWFLAKEMODEL_FILEOUT_H

#include <cstdio>
#include <cstdarg>

namespace SnowflakeModel
	{
	class FileOut
		{
		public:
			FileOut(const FileOut&)=delete;
			FileOut& operator=(const FileOut&)=delete;
			
			explicit FileOut(const char* dest)
				{
				file_out=fopen(dest,"wb");
				if(file_out==NULL)
					{throw "Could not open source file";}
				m_own=1;
				}

			
			explicit FileOut(FILE* dest)
				{
				file_out=dest;
				m_own=0;
				}
			
			~FileOut()
				{
				if(m_own)
					{::fclose(file_out);}
				}
			
			void putc(char ch_out)
				{::putc(ch_out,file_out);}
				
			void printf(const char* format,...)
				{
				va_list args;
				va_start(args, format);
				vfprintf(file_out, format, args);
				va_end(args);
				}
				
			
		private:
			FILE* file_out;
			bool m_own;
		};
	}

#endif
