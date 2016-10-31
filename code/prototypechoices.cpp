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
