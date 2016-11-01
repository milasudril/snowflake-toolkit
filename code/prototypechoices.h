//@	{
//@	 "targets":[{"name":"prototypechoices.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"prototypechoices.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_PROTOTYPECHOICES_H
#define SNOWFLAKEMODEL_PROTOTYPECHOICES_H

#include "prototypechoice.h"
#include <cassert>

namespace SnowflakeModel
	{
	class DataDump;

	class PrototypeChoices
		{
		public:
			explicit PrototypeChoices(const DataDump& dump,const char* name);

			PrototypeChoices():m_dist_dirty(0){}

			explicit PrototypeChoices(const char* filename)
				{append(filename);}

			const PrototypeChoice& choose(RandomGenerator& randgen) noexcept
				{
				if(m_dist_dirty)
					{
					auto n=m_probs.size();
					assert(n!=0);
					m_dist=std::discrete_distribution<size_t>(m_probs.data(),m_probs.data() + n);
					m_dist_dirty=0;
					}
				return m_choices[m_dist(randgen)];
				}

			void write(const char* key,DataDump& dump) const;

			void append(const char* filename);

			void append(const char* prototype_name,double probability
				,Twins<const DeformationData*> deformations)
				{
				m_choices.push_back(PrototypeChoice(m_solids,m_probs,prototype_name,probability,deformations));
				m_dist_dirty=1;
				}

			size_t choicesCount() const noexcept
				{return m_choices.size();}

		private:
			std::map<std::string,Solid> m_solids;
			std::vector<PrototypeChoice> m_choices;
			std::vector<double> m_probs;
			std::discrete_distribution<size_t> m_dist;
			bool m_dist_dirty;

		};
	}

#endif