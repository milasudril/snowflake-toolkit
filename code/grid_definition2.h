//@	{
//@	 "targets":[{"name":"grid_definition2.h","type":"include"}]
//@	}

#ifndef SNOWFLAKEMODEL_GRIDDEFINTION2_H
#define SNOWflAKEMODEL_GRIDDEFINTION2_H

#include "alice/stringkey.hpp"
#include "alice/typeinfo.hpp"

namespace SnowflakeModel
	{
	struct GridDefinition2
		{
		size_t N;
		double r_x;
		double r_y;
		double r_z;
		std::string filename;
		};
	}

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("grid definition 2")>
		{
		typedef SnowflakeModel::GridDefinition2 Type;

		static constexpr const char* descriptionGet() noexcept
			{
			return "";
			}
		};

	template<class ErrorHandler>
	struct MakeValue<SnowflakeModel::GridDefinition2,ErrorHandler>
		{
		static SnowflakeModel::GridDefinition2 make_value(const std::string& str);
		};

	template<class ErrorHandler>
	SnowflakeModel::GridDefinition2
	MakeValue<SnowflakeModel::GridDefinition2,ErrorHandler>::make_value(const std::string& str)
		{
		SnowflakeModel::GridDefinition2 ret{1,1.0,1.0,1.0,""};
		auto ptr=str.data();
		enum class State:int{N,RX,RY,RZ,FILENAME};
		auto state_current=State::N;
		std::string buffer;
		while(1)
			{
			auto ch_in=*ptr;
			switch(state_current)
				{
				case State::N:
					switch(ch_in)
						{
						case ',':
							ret.N=Alice::make_value<decltype(ret.N),ErrorHandler>(buffer);
							state_current=State::RX;
							buffer.clear();
							break;
						case '\0':
							ret.N=Alice::make_value<decltype(ret.N),ErrorHandler>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::RX:
					switch(ch_in)
						{
						case ':':
							ret.r_x=Alice::make_value<decltype(ret.r_x),ErrorHandler>(buffer);
							state_current=State::RY;
							buffer.clear();
							break;
						case '\0':
							ret.r_x=Alice::make_value<decltype(ret.r_x),ErrorHandler>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::RY:
					switch(ch_in)
						{
						case ':':
							ret.r_y=Alice::make_value<decltype(ret.r_y),ErrorHandler>(buffer);
							state_current=State::RY;
							buffer.clear();
							break;
						case '\0':
							ret.r_y=Alice::make_value<decltype(ret.r_y),ErrorHandler>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::RZ:
					switch(ch_in)
						{
						case ':':
							throw "Too many ratio values";
						case ',':
							ret.r_z=Alice::make_value<decltype(ret.r_z),ErrorHandler>(buffer);
							state_current=State::FILENAME;
							buffer.clear();
							break;
						case '\0':
							ret.r_z=Alice::make_value<decltype(ret.r_z),ErrorHandler>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::FILENAME:
					switch(ch_in)
						{
						case ',':
							throw "Too many arguments for grid definition";
						case '\0':
							return std::move(ret);
						default:
							ret.filename+=ch_in;					
						}
					break;
				}
			++ptr;
			}
		}
	}

#endif
