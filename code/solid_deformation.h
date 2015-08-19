#ifdef __WAND__
target[name[solid_deformation.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_SOLIDDEFORMATION_H
#define SNOWFLAKEMODEL_SOLIDDEFORMATION_H

#include "vector.h"
#include <string>
#include <map>

namespace SnowflakeModel
	{
	class SolidDeformation
		{
		public:
			SolidDeformation(const std::string& name):m_name(name)
				{}

			float operator[](size_t index) const
				{return *(&(m_matrix[0][0])+index);}

			float& operator[](size_t index)
				{return *(&(m_matrix[0][0])+index);}

			void parameterDefine(const std::string& name,size_t index
				,float value_default)
				{
				auto ip=parameter_map.insert({name,std::vector<size_t>{}});
				if(ip.second)
					{param_names.push_back(ip.first->first);}
				ip.first->second.push_back(index);
			//	auto ip=parameter_map.insert({name,index});
			//	if(!ip.second)
			//		{throw "Parameter name already used";}
				(*this)[index]=value_default;
				}

			const std::vector<size_t>* parameterFind(const std::string& name) const
				{
				auto ptr=parameter_map.find(name);
				if(ptr!=parameter_map.end())
					{return &ptr->second;}
				return nullptr;
				}

			const std::string& nameGet() const
				{return m_name;}

			const Matrix& matrixGet() const
				{return m_matrix;}

			const std::string* paramnamesBegin() const
				{return param_names.data();}

			const std::string* paramnamesEnd() const
				{return param_names.data()+param_names.size();}

		private:
			Matrix m_matrix;
			std::string m_name;
			std::map<std::string,std::vector<size_t> > parameter_map;
			std::vector<std::string> param_names;
		};
	};

#endif
