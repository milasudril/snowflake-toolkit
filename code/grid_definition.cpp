//@	{"targets":[{"name":"grid_definition.o","type":"object"}]}

#include "grid_definition.h"

using namespace SnowflakeModel;

namespace Alice
	{
	template<>
	GridDefinition make_value<GridDefinition>(const std::string& str)
		{
		GridDefinition ret{0,0,0,""};
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
							ret.N_x=make_value<decltype(ret.N_x)>(buffer);
							state_current=State::NY;
							buffer.clear();
							break;
						case '\0':
							ret.N_x=make_value<decltype(ret.N_x)>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::NY:
					switch(ch_in)
						{
						case ',':
							ret.N_y=make_value<decltype(ret.N_y)>(buffer);
							state_current=State::NZ;
							buffer.clear();
							break;
						case '\0':
							ret.N_y=make_value<decltype(ret.N_y)>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::NZ:
					switch(ch_in)
						{
						case ',':
							ret.N_z=make_value<decltype(ret.N_z)>(buffer);
							state_current=State::FILENAME;
							buffer.clear();
							break;
						case '\0':
							ret.N_z=make_value<decltype(ret.N_z)>(buffer);
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
				}
			++ptr;
			}
		}
	}
