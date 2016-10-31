//@	{"targets":[{"name":"deformation_data.o","type":"object"}]}

#include "deformation_data.h"
#include "datadump.h"
#include "stride_iterator.h"
#include "resourceobject.h"

using namespace SnowflakeModel;

static const char* keyFromDrawMethod(DrawMethod name)
	{
	if(name==custom_distribution)
		{return "custom";}
	if(name==delta_distribution)
		{return "delta";}
	if(name==exp_distribution)
		{return "exponential";}
	if(name==gamma_distribution)
		{return "gamma";}

	return "";
	}

inline bool hasMean(SnowflakeModel::DrawMethod m)
	{return m!=custom_distribution;}

DeformationData::DeformationData(const ResourceObject& obj)
	{
	if(obj.typeGet()!=ResourceObject::Type::ARRAY)
		{throw "Expected deformation data as an JSON array";}

	if(obj.objectCountGet()==0)
		{throw "No information availible about deformation";}

	auto distribution=static_cast<const char*>(obj.objectGet(static_cast<size_t>(0)));
	drawMethod=drawMethodFromName(distribution);
	name=distribution;
	if(drawMethod==custom_distribution)
		{
		if(obj.objectCountGet()!=1)
			{throw "Incorrect number of arguments for custom distribution";}
		auto src=static_cast<const char*>(obj.objectGet(1u));
		distribution_data=distributionLoad(src);
		distribution_src=src;	
		return;
		}

	if(hasMean(drawMethod))
		{
		if(obj.objectCountGet()<2u)
			{throw "No mean value given for distribution";}
		mean=static_cast<double>(obj.objectGet(1u));
		}

	if(hasStd(drawMethod))
		{
		if(obj.objectCountGet()<3u)
			{throw "No standard deviation given for distribution";}
		standard_deviation=static_cast<double>(obj.objectGet(2u));
		}
	}

void DeformationData::write(const char* key,DataDump& dump) const 
	{
	auto group=dump.groupCreate(key);
	std::string keyname(key);

	dump.write((keyname + "/mean").c_str(),&mean,1);
	dump.write((keyname + "/standard_deviation").c_str(),&standard_deviation,1);
	auto draw_method_name=keyFromDrawMethod(drawMethod);
	dump.write((keyname + "/drawMethod").c_str(),&draw_method_name,1);
	auto intervals=distribution_data.intervals();
	auto densities=distribution_data.densities();
	dump.write((keyname + "/distribution_data_intervals").c_str(),intervals.data(),intervals.size());
	dump.write((keyname + "/distribution_data_densities").c_str(),densities.data(),intervals.size());
	dump.write((keyname + "/distribution_src").c_str(),&distribution_src,1);
	}

DeformationData::DeformationData(const DataDump& dump,const char* name)
	{
	auto groupname=std::string(name);
	
	dump.arrayRead<decltype(mean)>((groupname +"/mean").c_str()).dataRead(&mean,1);
	dump.arrayRead<decltype(standard_deviation)>((groupname +"/standard_deviation").c_str())
		.dataRead(&standard_deviation,1);
	auto draw_method_name=std::move(dump.arrayGet<SnowflakeModel::DataDump::StringHolder>
		((groupname + "/drawMethod").c_str()).at(0));
	drawMethod=drawMethodFromName(draw_method_name);
	auto densities=dump.arrayGet<decltype(distribution_data)::result_type>
		((groupname + "/distribution_data_densities").c_str());
	auto intervals=dump.arrayGet<decltype(distribution_data)::result_type>
		((groupname + "/distribution_data_intervals").c_str());
	distribution_data=decltype(distribution_data)(intervals.data()
		,intervals.data() + intervals.size()
		,densities.data());
	distribution_src=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>
		((groupname + "/distribution_src").c_str()).at(0);
	}
