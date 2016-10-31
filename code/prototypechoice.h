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
			explicit PrototypeChoice(Solid&& solid,DeformationData&& deformation)=delete;

			explicit PrototypeChoice(const Solid& solid,DeformationData&& deformation):
				r_solid(&solid),m_deformation(deformation)
				{}

			explicit PrototypeChoice(std::map<std::string,Solid>& solids_loaded
				,std::vector<double>& probabilities
				,const ResourceObject& obj);
			
			const Solid& solidGet() const noexcept
				{return *r_solid;}

			const DeformationData& deformationGet() const noexcept
				{return m_deformation;}

		private:
			const Solid* r_solid;
			DeformationData m_deformation;
		};
	}

#endif
