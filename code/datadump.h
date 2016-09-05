//@	{
//@	"targets":[{"name":"datadump.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"datadump.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_DATADUMP_H
#define SNOWFLAKEMODEL_DATADUMP_H

#include <memory>
namespace H5
	{
	class CompType;
	class H5File;
	}

namespace SnowflakeModel
	{
    template <typename T, std::size_t N>
    constexpr std::size_t countof(T const (&)[N]) noexcept
		{return N;}

	template<typename T, typename U> constexpr size_t offsetof(U T::*member)
		{
		return (char*)&((T*)nullptr->*member) - (char*)nullptr;
		}

	class DataDump
		{
		public:
			DataDump(const char* filename);
			~DataDump();

			enum class Type:size_t{INTEGER,DOUBLE};

			struct FieldDescriptor
				{
				const char* name;
				size_t offset;
				Type type;
				};

			template<class T>
			void write(const char* objname,const T* data,size_t n_elems)
				{
				auto type=compoundMake(T::s_fields,countof(T::s_fields),sizeof(T));
				dataWrite(*type,objname,1,n_elems,data);
				}

		private:
			static void deleter(H5::CompType* obj);

			static std::unique_ptr<H5::CompType,void(*)(H5::CompType*)>
			compoundMake(const FieldDescriptor* fields
				,size_t n_fields,size_t struct_size);

			void dataWrite(const H5::CompType& type,const char* objname,size_t rank
				,size_t n_elems,const void* data);

			std::unique_ptr<H5::H5File> m_file;
		};
	}

#endif
