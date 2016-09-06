//@	{
//@	"targets":[{"name":"datadump.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"datadump.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_DATADUMP_H
#define SNOWFLAKEMODEL_DATADUMP_H

#include <memory>
#include <string>

namespace H5
	{
	class CompType;
	class H5File;
	class DataType;
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

	template<class T>
	struct MetaObject;

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
				auto type=compoundMake(MetaObject<T>::fields
					,countof(MetaObject<T>::fields),sizeof(T));
				dataWrite(*type,objname,n_elems,data);
				}

		private:
			static void deleter(H5::CompType* obj);

			static std::unique_ptr<H5::CompType,void(*)(H5::CompType*)>
			compoundMake(const FieldDescriptor* fields
				,size_t n_fields,size_t struct_size);

			void dataWrite(const H5::DataType& type,const char* objname
				,size_t n_elems,const void* data);

			void dataWrite(const H5::CompType& type,const char* objname
				,size_t n_elems,const void* data);

			std::unique_ptr<H5::H5File> m_file;
		};

	template<>
	void DataDump::write<char>(const char* objname,const char* data,size_t n_elems);

	template<>
	void DataDump::write<short>(const char* objname,const short* data,size_t n_elems);

	template<>
	void DataDump::write<int>(const char* objname,const int* data,size_t n_elems);

	template<>
	void DataDump::write<long>(const char* objname,const long* data,size_t n_elems);

	template<>
	void DataDump::write<long long>(const char* objname,const long long* data,size_t n_elems);

	template<>
	void DataDump::write<float>(const char* objname,const float* data,size_t n_elems);

	template<>
	void DataDump::write<double>(const char* objname,const double* data,size_t n_elems);



	template<>
	void DataDump::write<unsigned char>(const char* objname,const unsigned char* data,size_t n_elems);

	template<>
	void DataDump::write<unsigned short>(const char* objname,const unsigned short* data,size_t n_elems);

	template<>
	void DataDump::write<unsigned int>(const char* objname,const unsigned int* data,size_t n_elems);

	template<>
	void DataDump::write<unsigned long>(const char* objname,const unsigned long* data,size_t n_elems);

	template<>
	void DataDump::write<unsigned long long>(const char* objname,const unsigned long long* data,size_t n_elems);

	template<>
	void DataDump::write<std::string>(const char* objname,const std::string* data,size_t n_elems);
	}

#endif
