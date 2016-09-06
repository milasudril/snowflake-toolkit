//@	{
//@	"targets":[{"name":"datadump.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"datadump.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_DATADUMP_H
#define SNOWFLAKEMODEL_DATADUMP_H

#include <memory>
#include <string>
#include <type_traits>

namespace H5
	{
	class H5File;
	class DataType;
	class Group;
	class StrType;
	}

namespace SnowflakeModel
	{
    template <typename T, std::size_t N>
    constexpr std::size_t countOf(T const (&)[N]) noexcept
		{return N;}

	template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
		{
		return (char*)&((T*)nullptr->*member) - (char*)nullptr;
		}

	class DataDump
		{
		public:
			typedef std::unique_ptr<H5::DataType,void(*)(H5::DataType*)> DataTypeHandle;
			typedef std::unique_ptr<H5::Group,void(*)(H5::Group*)> GroupHandle;

			DataDump(const char* filename);
			~DataDump();

			struct FieldDescriptor
				{
				const char* name;
				size_t offset;
				const H5::DataType& type;
				};

			template<class T,class Enable=void>
			class MetaObject
				{
				public:
					const H5::DataType& typeGet() const noexcept;
				};

			template<class T>
			class MetaObject<T,typename std::enable_if<std::is_same<T,const char*>::value>::type>
				{
				public:
					MetaObject();
					const H5::DataType& typeGet() const noexcept
						{return r_cstr;}
				private:
					const H5::DataType& r_cstr;
				};

			template<class T>
			class MetaObject<T,typename std::enable_if<std::is_class<T>::value>::type>
				{
				public:
					static const DataDump::FieldDescriptor fields[];
					MetaObject():m_type(compoundMake(fields,countOf(fields),sizeof(T)))
						{}

					const H5::DataType& typeGet() const noexcept
						{return *m_type;}

				private:
					DataTypeHandle m_type;
				};

			template<class T>
			void write(const char* objname,const T* data,size_t n_elems)
				{
				dataWrite(MetaObject<T>().typeGet(),objname,n_elems,data);
				}

			static 
			DataTypeHandle compoundMake(const FieldDescriptor* fields
				,size_t n_fields,size_t struct_size);

			GroupHandle groupCreate(const char* name);

		private:
			static void deleter(H5::DataType* obj);
			static void deleter(H5::Group* obj);

			void dataWrite(const H5::DataType& type,const char* objname
				,size_t n_elems,const void* data);

			std::unique_ptr<H5::H5File> m_file;
			static H5::StrType s_cstr;
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
