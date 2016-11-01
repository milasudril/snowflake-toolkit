//@	{
//@	 "targets":[{"name":"prototypechoice.h","type":"include"}]
//@ ,"dependencies_extra":[{"ref":"prototypechoice.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_PROTOTYPECHOICE_H
#define SNOWFLKAEMODEL_PROTOTYPECHOICE_H

#include "deformation_data.h"
#include <vector>
#include <map>
#include <string>

namespace SnowflakeModel
	{
	class ResourceObject;
	class Solid;

	class PrototypeChoice
		{
		public:
			explicit PrototypeChoice(Solid&& solid)=delete;

			explicit PrototypeChoice(const Solid& solid):r_solid(&solid)
				{}

			explicit PrototypeChoice(std::map<std::string,Solid>& solids_loaded
				,std::vector<double>& probabilities
				,const ResourceObject& obj);

			explicit PrototypeChoice(std::map<std::string,Solid>& solids_loaded
				,std::vector<double>& probabilities,const char* prototype_name,double probability
				,Twins<const DeformationData*> deformations);
			
			const Solid& solidGet() const noexcept
				{return *r_solid;}

			const DeformationData* deformationsBegin() const noexcept
				{return m_deformations.data();}

			const DeformationData* deformationsEnd() const noexcept
				{return m_deformations.data() + m_deformations.size();}

			size_t deformationsCount() const noexcept
				{return m_deformations.size();}

			void deformationAppend(DeformationData&& deformation)
				{m_deformations.push_back(std::move(deformation));}

		private:
			const Solid* r_solid;
			std::vector<DeformationData> m_deformations;
		};
	}

#endif
