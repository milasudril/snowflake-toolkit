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

SolidDeformation::SolidDeformation(const DataDump& dump,const char* id)
	{
	auto group=dump.groupOpen(id);
	std::string group_name(id);
	dump.matrixGet((group_name + "/matrix").c_str(),&m_matrix[0][0],4,4);
	m_name=dump.arrayGet<DataDump::StringHolder>((group_name + "/name").c_str())
		.at(0);

		{
		auto param_name_base=group_name+"/parameter_map";
		auto group_params=dump.groupOpen(param_name_base.c_str());
		param_name_base+='/';
		dump.iterate(*group_params,[this,&param_name_base,&dump]
			(const char* group_name)
			{
			auto param_name=param_name_base+group_name;
			parameter_map[group_name]=dump.arrayGet<size_t>(param_name.c_str());
			});
		}
	
	}