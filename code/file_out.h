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

			enum class OpenMode:int{TRUNCATE,APPEND};
			
			explicit FileOut(const char* dest,OpenMode mode=OpenMode::TRUNCATE)
				{
				file_out=fopen(dest,mode==OpenMode::TRUNCATE?"wb":"ab");
				if(file_out==NULL)
					{throw "Could not open source file";}
				m_own=1;
				}
			
			explicit FileOut(FILE* dest)
				{
				file_out=dest;
				m_own=0;
				}
			
			~FileOut() noexcept
				{
				flush();
				if(m_own)
					{::fclose(file_out);}
				}
			
			void putc(char ch_out) noexcept
				{::putc(ch_out,file_out);}
				
			void printf(const char* format,...) noexcept
				{
				va_list args;
				va_start(args, format);
				vfprintf(file_out, format, args);
				va_end(args);
				}

			FILE* handleGet() noexcept
				{return file_out;}

			void flush()
				{fflush(file_out);}
			
		private:
			FILE* file_out;
			bool m_own;
		};
	}

#endif
