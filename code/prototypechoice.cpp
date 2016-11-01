//@	{"targets":[{"name":"prototypechoice.o","type":"object"}]}

#include "prototypechoice.h"
#include "resourceobject.h"
#include "file_in.h"
#include "solid_loader.h"
#include "config_parser.h"
#include "solid.h"

using namespace SnowflakeModel;

static Solid solid_load(const char* filename)
	{
	SnowflakeModel::Solid solid_in;
		{
		SnowflakeModel::FileIn file_in(filename);
		SnowflakeModel::ConfigParser parser(file_in);
		SnowflakeModel::SolidLoader loader(solid_in);
		parser.commandsRead(loader);
		}

	return std::move(solid_in);
	}

PrototypeChoice::PrototypeChoice(std::map<std::string,Solid>& solids_loaded
	,std::vector<double>& probabilities,const char* prototype_name,double probability
	,Twins<const DeformationData*> deformations)
	{
	auto i=solids_loaded.find(prototype_name);
	i=i==solids_loaded.end()?
		solids_loaded.insert({prototype_name,solid_load(prototype_name)}).first
		:i;
	r_solid=&i->second;
	while(deformations.first!=deformations.second)
		{
		m_deformations.push_back(*deformations.first);
		++deformations.first;
		}
	probabilities.push_back(probability);
	}

PrototypeChoice::PrototypeChoice(std::map<std::string,Solid>& solids_loaded
	,std::vector<double>& probabilities
	,const ResourceObject& obj)
	{	
	auto prototype=static_cast<const char*>(obj.objectGet("prototype"));
	auto i=solids_loaded.find(prototype);
	i=i==solids_loaded.end()?
		solids_loaded.insert({prototype,solid_load(prototype)}).first
		:i;
	r_solid=&i->second;
	
	auto deformations=obj.objectGet("deformations");
	if(deformations.typeGet()!=ResourceObject::Type::ARRAY)
		{throw "Deformations has to be an array";}
	auto def_count=deformations.objectCountGet();
	for(decltype(def_count) k=0;k<def_count;++k)
		{m_deformations.push_back(DeformationData(deformations.objectGet(k)));}

	probabilities.push_back(static_cast<double>(obj.objectGet("probability")));
	}

