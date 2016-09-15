//@	{"targets":[{"name":"idgenerator.hpp","type":"include"}]}

#ifndef SNOWFLAKEMODEL_IDGENERATOR_H
#define SNOWFLAKEMODEL_IDGENERATOR_H

#include "datadump.h"
#include <vector>

namespace SnowflakeModel
	{
	template<class IdType>
	class IdGenerator
		{
		public:
			IdGenerator(const DataDump& dump,const char* id)
				{
				auto gropu=dump.groupOpen(id);
				auto group_name=std::string(id);
				m_freelist=dump.arrayGet<IdType>((group_name + "/freelist").c_str());
				m_id_next=dump.arrayGet<IdType>((group_name + "/id_current").c_str()).at(0);
				}

			IdGenerator() noexcept:m_id_next(-1){}

			IdType idGet() noexcept
				{
				if(m_freelist.empty())
					{
					++m_id_next;
					return m_id_next;
					}
				auto ret=std::move(m_freelist.back());
				m_freelist.pop_back();
				return ret;
				}

			IdGenerator& idRelease(const IdType& id)
				{
				m_freelist.push_back(id);
				return *this;
				}

			IdGenerator& reset() noexcept
				{
				m_freelist.clear();
				m_id_next=-1;
				return *this;
				}

			void write(const char* id,DataDump& dump) const
				{
				auto group=dump.groupCreate(id);
				auto group_name=std::string(id);
				dump.write((group_name + "/freelist").c_str(),m_freelist.data()
					,m_freelist.size());
				dump.write((group_name + "/id_current").c_str(),&m_id_next
					,1);
				}

		private:
			std::vector<IdType> m_freelist;
			IdType m_id_next;
		};
	}

#endif
