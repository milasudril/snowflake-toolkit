//@	{
//@	 "targets":[{"name":"prototypechoices.o","type":"object"}]
//@	}

#include "prototypechoices.h"
#include "resourceobject.h"
#include "solid.h"

using namespace SnowflakeModel;

PrototypeChoices::PrototypeChoices(const ResourceObject& obj)
	{
	if(obj.typeGet()!=ResourceObject::Type::ARRAY)
		{throw "Expected an array of prototype choices";}

	auto n_objs=obj.objectCountGet();
	std::vector<double> probs;
	probs.reserve(n_objs);
	for(decltype(n_objs) k=0;k<n_objs;++k)
		{
		m_choices.push_back(PrototypeChoice(m_solids,probs,obj.objectGet(k)));
		}

	m_dist=std::discrete_distribution<size_t>(probs.data(),probs.data() + n_objs);
	}

void PrototypeChoices::write(const char* key,DataDump& dump) const
	{
	auto group=dump.groupCreate(key);
	std::string keyname(key);

	
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
		//	choices_begin->write(group_name_current.c_str(),dump);
			++choices_begin;
			++k;
			}
		}

	auto probs=m_dist.probabilities();
	dump.write((keyname + "/dist").c_str(),probs.data(),probs.size());

		{
		size_t k=0;
		auto solids_begin=m_solids.begin();
		auto solids_end=m_solids.end();
		auto solids=dump.groupCreate((keyname + "/śolids").c_str());
		auto group_name=keyname + "/solids/";
		while(solids_begin!=solids_end)
			{
			auto group_name_current=group_name + solids_begin->first;
			solids_begin->second.write(group_name_current.c_str(),dump);
			++solids_begin;
			++k;
			}
		}

	}
