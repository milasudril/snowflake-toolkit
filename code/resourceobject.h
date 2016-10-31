//@	{
//@	 "targets":[{"name":"resourceobject.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"resourceobject.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_RESOURCEOBJECT_H
#define SNOWFLAKEMODEL_RESOURCEOBJECT_H

#include <utility>
#include <cstddef>

namespace SnowflakeModel
	{
	class DataSource;

	class ResourceObject
		{
		public:
			class Iterator
				{
				public:
					Iterator(const Iterator&)=delete;
					Iterator& operator=(const Iterator&)=delete;

					Iterator(Iterator&& i) noexcept:
						r_object(i.r_object),m_handle(i.m_handle)
						{i.m_handle=nullptr;}
					Iterator& operator=(Iterator&& i)=delete;

					Iterator(ResourceObject& object);
					void next() noexcept;
					std::pair<const char*,ResourceObject> get() noexcept;
					bool endAt() noexcept;

				private:
					ResourceObject& r_object;
					void* m_handle;

				};

			ResourceObject(const ResourceObject&)=delete;
			ResourceObject& operator=(const ResourceObject&)=delete;

			explicit ResourceObject(DataSource&& reader):ResourceObject(reader)
				{}

			explicit ResourceObject(DataSource& reader);

			~ResourceObject();

			ResourceObject(ResourceObject&& tree) noexcept:m_handle(tree.m_handle)
				{tree.m_handle=nullptr;}

			ResourceObject& operator=(ResourceObject&& tree) noexcept
				{
				std::swap(tree.m_handle,m_handle);
				return *this;
				}

			enum class Type:unsigned int{OBJECT,ARRAY,STRING,INTEGER,FLOAT};

			Type typeGet() const noexcept;

			ResourceObject objectGet(const char* name) const;
			bool objectExists(const char* name) const noexcept;
			size_t objectCountGet() const noexcept;
			ResourceObject objectGet(size_t index) const;
			Iterator objectIteratorGet() noexcept
				{
				Iterator i(*this);
				return std::move(i);
				}

			const char* stringGet(const char* name) const noexcept;
			const char* stringGet() const noexcept;
			long long int integerGet() const noexcept;
			double floatGet() const noexcept;

			explicit operator const char*() const;
			explicit operator long long int() const;
			explicit operator double() const;

		private:
			ResourceObject(void* handle,const char* name);
			void* m_handle;
		};
	}
#endif
