//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"file_in.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_FILEIN_H
#define SNOWFLAKEMODEL_FILEIN_H

#include <cstdio>
#include <string>

namespace SnowflakeModel
	{
	class FileIn
		{
		public:
			FileIn(const FileIn&)=delete;
			FileIn& operator=(const FileIn&)=delete;

			explicit FileIn(const char* source)
				{
				fprintf(stderr,"# Loading %s\n",source);
				file_in=fopen(source,"rb");
				if(file_in==NULL)
					{throw "Could not open source file";}
				m_filename=source;
				}


			explicit FileIn(FILE* source)
				{
				file_in=source;
				}

			const std::string& filenameGet() const
				{return m_filename;}

			~FileIn()
				{
				if(m_filename!="")
					{::fclose(file_in);}
				}

			int getc()
				{return ::getc(file_in);}

			FILE* handleGet() noexcept
				{return file_in;}

		private:
			FILE* file_in;
			std::string m_filename;
		};
	}

#endif
