//@	{
//@	 "targets":[{"name":"deformation_data.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"deformation_data.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKE_MODEL_DEFORMATIONDATA_H
#define SNOWFLAKE_MODEL_DEFORMATIONDATA_H

#include "randomgenerator.h"
#include "file_in.h"
#include "xytable.h"
#include "stride_iterator.h"
#include <memory>
#include <string>
#include <alice/alice.hpp>

namespace SnowflakeModel
	{
	struct DeformationData;
	class DataDump;
	class ResourceObject;

	typedef float (*DrawMethod)(const DeformationData& obj,RandomGenerator& randgen);

	struct DeformationData
		{
		DeformationData():mean(1.0f),standard_deviation(1.0f){}
		explicit DeformationData(const char* in_dir,const ResourceObject& obj);
		explicit DeformationData(const DataDump& dump,const char* name);

		std::string name;
		float mean;
		float standard_deviation;
		std::string distribution_src;
		mutable std::piecewise_linear_distribution<float> distribution_data;
		DrawMethod drawMethod;

		void write(const char* key,DataDump& dump) const;
		};
	}


static std::piecewise_linear_distribution<float> distributionLoad(const std::string& str)
	{
	auto table=xytable(SnowflakeModel::FileIn(str.c_str()));
	SnowflakeModel::StrideIterator<decltype(table)::value_type,0> x(table.data());
	SnowflakeModel::StrideIterator<decltype(table)::value_type,1> y(table.data());
	
	return std::piecewise_linear_distribution<float>(x,x+table.size(),y);
	}

static float custom_distribution(const SnowflakeModel::DeformationData& obj,SnowflakeModel::RandomGenerator& randgen)
	{return obj.distribution_data(randgen);}

static float delta_distribution(const SnowflakeModel::DeformationData& obj,SnowflakeModel::RandomGenerator& randgen)
	{return obj.mean;}

static float exp_distribution(const SnowflakeModel::DeformationData& obj,SnowflakeModel::RandomGenerator& randgen)
	{
	std::exponential_distribution<float> e(1.0f/obj.mean);
	return e(randgen);
	}

static float gamma_distribution(const SnowflakeModel::DeformationData& obj,SnowflakeModel::RandomGenerator& randgen)
	{
//	Parameter setup from Wikipedia article.
//	Use the standard deviation from user input:
//		sigma^2=k*theta^2
//	we have
	float E=obj.mean;
	float sigma=obj.standard_deviation;
	float k=E*E/(sigma*sigma);
	float theta=sigma*sigma/E;
//	Convert to C++ notation
	auto beta=theta;
	float alpha=k;

	std::gamma_distribution<float> G(alpha,beta);
	return G(randgen);
	}

static SnowflakeModel::DrawMethod drawMethodFromName(const char* name)
	{
	switch(Alice::Stringkey(name))
		{
		case Alice::Stringkey("custom"):
			return custom_distribution;
		case Alice::Stringkey("delta"):
			return delta_distribution;
		case Alice::Stringkey("exponential"):
			return exp_distribution;
		case Alice::Stringkey("gamma"):
			return gamma_distribution;
		}
	throw "Unknown distribution";
	}	

inline bool hasStd(SnowflakeModel::DrawMethod m)
	{
	return m==gamma_distribution;
	}

namespace Alice
	{
	template<class ErrorHandler>
	struct MakeValue<SnowflakeModel::DeformationData,ErrorHandler>
		{
		static SnowflakeModel::DeformationData make_value(const std::string& str)
			{
			SnowflakeModel::DeformationData ret;
			ret.mean=1.0f;
			ret.standard_deviation=1.0f;
			ret.drawMethod=delta_distribution;
			auto ptr=str.c_str();
			enum class State:int{INIT,DISTRIBUTION,DISTRIBUTION_CUSTOM,VALUE_1,VALUE_2};
			auto state_current=State::INIT;
			std::string word_current;
			while(1)
				{
				auto ch_in=*ptr;
				switch(state_current)
					{
					case State::INIT:
						switch(ch_in)
							{
							case ',':
								ret.name=word_current;
								state_current=State::DISTRIBUTION;
								word_current.clear();
								break;

							case '\0':
								throw "Too few arguments for deformation";

							default:
								word_current+=ch_in;
							}
						break;

					case State::DISTRIBUTION:
						switch(ch_in)
							{
							case ',':
								ret.drawMethod=drawMethodFromName(word_current.c_str());
								word_current.clear();
								state_current=ret.drawMethod==custom_distribution?
									State::DISTRIBUTION_CUSTOM:State::VALUE_1;
								break;

							case '\0':
								throw "Too few arguments for deformation";

							default:
								word_current+=ch_in;
							}
						break;

					case State::DISTRIBUTION_CUSTOM:
						switch(ch_in)
							{
							case ',':
								throw "Custom distributions only take one argument";
							case '\0':
								ret.distribution_data=distributionLoad(word_current);
								ret.distribution_src=word_current;
								return std::move(ret);
							default:
								word_current+=ch_in;
							}
						break;

					case State::VALUE_1:
						switch(ch_in)
							{
							case ',':
								if(!hasStd(ret.drawMethod))
									{throw "The current distribution does not accept a standard distribution";}
								ret.mean=atof(word_current.c_str());
								word_current.clear();
								state_current=State::VALUE_2;
								break;

							case '\0':
								if(hasStd(ret.drawMethod))
									{throw "The current distribution requires a standard deviation";}
								ret.mean=atof(word_current.c_str());
								return std::move(ret);

							default:
								word_current+=ch_in;
							}
						break;


					case State::VALUE_2:
						switch(ch_in)
							{
							case ',':
								throw "Too many arguments for distribution";

							case '\0':
								ret.standard_deviation=atof(word_current.c_str());
								return std::move(ret);

							default:
								word_current+=ch_in;
							}
						break;
					};
				++ptr;
				}
			return std::move(ret);
			}
		};
	}

#endif
