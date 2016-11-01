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
	,std::vector<double>& probabilities
	,const ResourceObject& obj)
	{
	if(obj.typeGet()!=ResourceObject::Type::ARRAY)
		{throw "Object is not an array";}

	if(obj.objectCountGet()!=3)
		{throw "Invalid number of fields in table";}
	
	auto name=static_cast<const char*>(obj.objectGet(static_cast<size_t>(0)));
	auto i=solids_loaded.find(name);
	i=i==solids_loaded.end()?
		solids_loaded.insert({name,solid_load(name)}).first
		:i;
	r_solid=&i->second;
	m_deformation=DeformationData(obj.objectGet(2u));
	probabilities.push_back(static_cast<double>(obj.objectGet(1u)));

	}

