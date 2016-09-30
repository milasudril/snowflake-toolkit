//@	{
//@	 "targets":[{"name":"grid_definition2.o","type":"object"}]
//@	}

#include "grid_definition2.h"

using namespace SnowflakeModel;

namespace Alice
	{
	template<>
	GridDefinition2 make_value<GridDefinition2>(const std::string& str)
		{
		GridDefinition2 ret{1,1.0,1.0,1.0,""};
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
							ret.N=make_value<long long int>(buffer);
							state_current=State::RX;
							buffer.clear();
							break;
						case '\0':
							ret.N=make_value<long long int>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::RX:
					switch(ch_in)
						{
						case ':':
							ret.r_x=make_value<decltype(ret.r_x)>(buffer);
							state_current=State::RY;
							buffer.clear();
							break;
						case '\0':
							ret.r_x=make_value<decltype(ret.r_x)>(buffer);
							return std::move(ret);
						default:
							buffer+=ch_in;
						}
					break;

				case State::RY:
					switch(ch_in)
						{
						case ':':
							ret.r_y=make_value<decltype(ret.r_y)>(buffer);
							state_current=State::RY;
							buffer.clear();
							break;
						case '\0':
							ret.r_y=make_value<decltype(ret.r_y)>(buffer);
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
							ret.r_z=make_value<decltype(ret.r_z)>(buffer);
							state_current=State::FILENAME;
							buffer.clear();
							break;
						case '\0':
							ret.r_z=make_value<decltype(ret.r_z)>(buffer);
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
