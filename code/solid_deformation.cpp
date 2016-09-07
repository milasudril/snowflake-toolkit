//@	{"targets":[{"name":"solid_deformation.o","type":"object"}]}

#include "solid_deformation.h"

using namespace SnowflakeModel;

void SolidDeformation::write(const char* id,DataDump& dump) const
	{
	auto group=dump.groupCreate(id);
	std::string group_name(id);
	dump.write((group_name+"/matrix").c_str(),&m_matrix[0][0],4,4);
	dump.write((group_name+"/name").c_str(),&m_name,1);

		{
		auto group_params=dump.groupCreate((group_name+"/parameter_map").c_str());
		std::string param_name_base=group_name+"/parameter_map/";
		auto params_begin=parameter_map.begin();
		auto params_end=parameter_map.end();
		while(params_begin!=params_end)
			{
			dump.write((param_name_base+params_begin->first).c_str()
				,params_begin->second.data(),params_begin->second.size());
			++params_begin;
			}
		}
	}
