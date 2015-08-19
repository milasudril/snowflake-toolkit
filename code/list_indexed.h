#ifdef __WAND__
target[name[list_indexed.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_LISTINDEXED_H
#define SNOWFLAKEMODEL_LISTINDEXED_H

#include <vector>
#include <list>
#include <cassert>

namespace SnowflakeModel
	{
	template<class T>
	class ListIndexed
		{
		public:
			ListIndexed(const ListIndexed&)=delete;
			ListIndexed& operator=(const ListIndexed&)=delete;
			
			ListIndexed(){}
			
			T& append(const T& obj)
				{
				auto X=new T(obj);
				m_index.push_back(X);
				return *X;
				}

			T& operator[](size_t index)
				{return *m_index[index];}

			const T& operator[](size_t index) const
				{return *m_index[index];}

			size_t length() const
				{return m_index.size();}

			void remove(size_t index)
				{
				delete m_index[index];
				m_index.erase(m_index.begin()+index);
				}
				
			T** begin()
				{return m_index.data();}
			
			T** end()
				{return m_index.data()+length();}
				
			T* const* begin() const
				{return m_index.data()+length();}
			
			T* const* end() const
				{return m_index.data()+length();}
			
			~ListIndexed()
				{
				auto i=m_index.data();
				auto end=m_index.data()+length();
				while(i!=end)
					{
					delete *i;
					++i;
					}
				}


		private:
			std::vector<T*> m_index;
		};
	}

#endif
