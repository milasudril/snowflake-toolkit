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

#include "datasource.h"
#include <cstdio>
#include <string>

namespace SnowflakeModel
	{
	class FileIn:public DataSource
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

			FileIn(FileIn&& f) noexcept:file_in(f.file_in)
				{std::swap(f.m_filename,m_filename);}

			FileIn& operator=(FileIn&& f) noexcept
				{
				std::swap(file_in,f.file_in);
				std::swap(m_filename,f.m_filename);
				return *this;
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

			size_t read(void* buffer, size_t length)
				{return fread(buffer,1,length,file_in);}

			int getc()
				{return ::getc(file_in);}

			const char* nameGet() const noexcept
				{return m_filename.data();}

		private:
			FILE* file_in;
			std::string m_filename;
		};
	}

#endif
