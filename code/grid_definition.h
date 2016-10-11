//@	{
//@	 "targets":[{"name":"grid_definition.h","type":"include"}]
//@	}

#ifndef SNOWFLAKEMODEL_GRIDDEFINTION_H
#define SNOWflAKEMODEL_GRIDDEFINTION_H

#include "alice/stringkey.hpp"
#include "alice/typeinfo.hpp"

namespace SnowflakeModel
	{
	struct GridDefinition
		{
		unsigned int N_x;
		unsigned int N_y;
		unsigned int N_z;
		std::string filename;
		};
	}

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("grid definition")>
		{
		typedef SnowflakeModel::GridDefinition Type;
		static constexpr const char* descriptionGet() noexcept
			{
			return "";
			}
		};

	template<class ErrorHandler>
	struct MakeValue<SnowflakeModel::GridDefinition,ErrorHandler>
		{
		static SnowflakeModel::GridDefinition make_value(const std::string& str);
		};

	template<class ErrorHandler>
	SnowflakeModel::GridDefinition
	MakeValue<SnowflakeModel::GridDefinition,ErrorHandler>::make_value(const std::string& str)
		{
		SnowflakeModel::GridDefinition ret{0,0,0,""};
		auto ptr=str.data();
		enum class State:int{NX,NY,NZ,FILENAME};
		auto state_current=State::NX;
		std::string buffer;
		while(1)
			{
			auto ch_in=*ptr;
			switch(state_current)
				{
				case State::NX:
					switch(ch_in)
						{
						case ',':
							ret.N_x=Alice::make_value<decltype(ret.N_x),ErrorHandler>(buffer);
							state_current=State::NY;
							buffer.clear();
							break;
						case '\0':
							ret.N_x=Alice::make_value<decltype(ret.N_x),ErrorHandler>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::NY:
					switch(ch_in)
						{
						case ',':
							ret.N_y=Alice::make_value<decltype(ret.N_y),ErrorHandler>(buffer);
							state_current=State::NZ;
							buffer.clear();
							break;
						case '\0':
							ret.N_y=Alice::make_value<decltype(ret.N_y),ErrorHandler>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::NZ:
					switch(ch_in)
						{
						case ',':
							ret.N_z=Alice::make_value<decltype(ret.N_z),ErrorHandler>(buffer);
							state_current=State::FILENAME;
							buffer.clear();
							break;
						case '\0':
							ret.N_z=Alice::make_value<decltype(ret.N_z),ErrorHandler>(buffer);
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
