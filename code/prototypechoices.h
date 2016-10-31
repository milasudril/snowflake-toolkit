//@	{
//@	 "targets":[{"name":"prototypechoices.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"prototypechoices.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_PROTOTYPECHOICES_H
#define SNOWFLAKEMODEL_PROTOTYPECHOICES_H

#include "prototypechoice.h"

namespace SnowflakeModel
	{
	class DataDump;

	class PrototypeChoices
		{
		public:
			explicit PrototypeChoices(const DataDump& dump,const char* name);

			explicit PrototypeChoices(const ResourceObject& obj);

			const PrototypeChoice& choose(RandomGenerator& randgen) noexcept
				{return m_choices[m_dist(randgen)];}

			void write(const char* key,DataDump& dump) const;

		private:
			std::map<std::string,Solid> m_solids;
			std::vector<PrototypeChoice> m_choices;
			std::discrete_distribution<size_t> m_dist;
		};
	}

#endif