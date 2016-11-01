//@	{
//@	 "targets":[{"name":"prototypechoices.o","type":"object"}]
//@	}

#include "prototypechoices.h"
#include "resourceobject.h"
#include "solid.h"
#include "file_in.h"

using namespace SnowflakeModel;

void PrototypeChoices::append(const char* filename)
	{
	ResourceObject obj{FileIn(filename)};
	if(obj.typeGet()!=ResourceObject::Type::ARRAY)
		{throw "Expected an array of prototype choices";}

	auto n_objs=obj.objectCountGet();
	for(decltype(n_objs) k=0;k<n_objs;++k)
		{m_choices.push_back(PrototypeChoice(m_solids,m_probs,obj.objectGet(k)));}
	m_dist_dirty=1;
	}

PrototypeChoices::PrototypeChoices(const DataDump& dump,const char* name)
	{
	std::string keyname(name);
	auto probs=dump.arrayGet<double>((keyname + "/dist").c_str());
		{
		auto group_name=keyname + "/choices";
		auto group=dump.groupOpen(group_name.c_str());
		group_name+='/';

		{
		auto group_name=keyname + "/solids";
		auto group=dump.groupOpen(group_name.c_str());
		group_name+='/';

		dump.iterate(*group,[&dump,&group_name,this]
			(const char* name)
			{
			auto group_name_current=group_name + name;
			m_solids.insert({std::string(name),Solid(dump,group_name_current.c_str())});
			});
		}

		dump.iterate(*group,[&dump,&group_name,this]
			(const char* name)
			{
			auto group_name_current=group_name + name;
			auto g=dump.groupOpen(group_name_current.c_str());
			uintptr_t solid_id;
			dump.arrayRead<uintptr_t>((group_name_current + "/solid").c_str())
				.dataRead(&solid_id,1);
			auto id_str=std::to_string(solid_id);
			auto i=m_solids.find(id_str);
			if(i==m_solids.end())
				{throw "Solid not found";}
			auto defgroup_name=group_name_current + "/deformations";
			auto g2=dump.groupOpen(defgroup_name.c_str());
			defgroup_name+='/';
			PrototypeChoice choice(i->second);
			dump.iterate(*g2,[&dump,&choice,&defgroup_name,this](const char* name)
				{
				choice.deformationAppend(DeformationData(dump,(defgroup_name + name).c_str()));
				});
			m_choices.push_back( std::move(choice) );
			});
		}
	m_dist_dirty=1;
	}

void PrototypeChoices::write(const char* key,DataDump& dump) const
	{
	auto group=dump.groupCreate(key);
	std::string keyname(key);

	auto probs=m_dist.probabilities();
	dump.write((keyname + "/dist").c_str(),probs.data(),probs.size());	

		{
		auto solids_begin=m_solids.begin();
		auto solids_end=m_solids.end();
		auto solids=dump.groupCreate((keyname + "/solids").c_str());
		auto group_name=keyname + "/solids/";
		while(solids_begin!=solids_end)
			{
			char id[32];
			sprintf(id,"%lu",reinterpret_cast<uintptr_t>(&solids_begin->second));
			auto group_name_current=group_name + id;
			solids_begin->second.write(group_name_current.c_str(),dump);
			++solids_begin;
			}
		}

		{
		size_t k=0;
		auto choices_begin=m_choices.data();
		auto choices_end=choices_begin + m_choices.size();
		auto choices=dump.groupCreate((keyname + "/choices").c_str());
		auto group_name=keyname + "/choices/";
		while(choices_begin!=choices_end)
			{
			char id[32];
			sprintf(id,"%016zx",k);
			auto group_name_current=group_name + id;
			auto g=dump.groupCreate(group_name_current.c_str());
			auto id_solid=reinterpret_cast<uintptr_t>(&choices_begin->solidGet());
			dump.write((group_name_current+"/solid").c_str(),&id_solid,1);
			auto defgroup_name=group_name_current+"/deformations";
			auto defgroup=dump.groupCreate(defgroup_name.c_str());
			defgroup_name+='/';
			auto defs_begin=choices_begin->deformationsBegin();
			auto defs_end=choices_begin->deformationsEnd();
			size_t l=0;
			while(defs_begin!=defs_end)
				{
				char id2[32];
				sprintf(id2,"%016zx",l);
				defs_begin->write((defgroup_name + id2).c_str(),dump);
				++defs_begin;
				++l;
				}
			++choices_begin;
			++k;
			}
		}

	}
