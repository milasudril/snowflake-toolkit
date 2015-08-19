#ifdef __WAND__
target[name[config_parser.o] type[object]]
#endif

#include "config_parser.h"
#include "config_commandhandler.h"
#include "config_command.h"
#include <stack>

using namespace SnowflakeModel;

ConfigParser::ConfigParser(FileIn& source):r_source(source)
	{}
	
namespace
	{
	enum class State:int{NEWLINE,STATEMENT,ESCAPE,COMMENT};
	}

void ConfigParser::commandsRead(ConfigCommandHandler& handler)
	{
	std::stack<ConfigCommand> command_stack;
	ConfigCommand cmd_current;
	std::string token_in;
	int ch_in;
	auto state_current=State::NEWLINE;
	while((ch_in=r_source.getc())!=EOF)
		{
		if(state_current==State::NEWLINE)
			{
			if(!(ch_in>='\0' && ch_in<=' '))
				{
				switch(ch_in)
					{
					case '#':
						state_current=State::COMMENT;
						break;
					default:
						state_current=State::STATEMENT;
					}
				}
			}
		switch(state_current)
			{
			case State::NEWLINE:
				break;
				
			case State::STATEMENT:
				switch(ch_in)
					{
					case '#':
						state_current=State::COMMENT;
						break;
						
					case '(':
						command_stack.push(cmd_current);
						cmd_current.m_name=token_in;
						cmd_current.m_arguments.clear();
						token_in.clear();
						state_current=State::NEWLINE;
						break;
						
					case ',':
						cmd_current.m_arguments.push_back(token_in);
						token_in.clear();
						state_current=State::NEWLINE;
						break;
						
					case ')':
						{
						if(command_stack.size()==0)
							{
							throw "Too many ')' in input stream";
							break;
							}
						if(token_in.size()!=0)
							{cmd_current.m_arguments.push_back(token_in);}
						
						auto result=handler.invoke(cmd_current,r_source);
						
						if(command_stack.size()>1)
							{token_in=result;}
						else
							{token_in.clear();}
						
						cmd_current=command_stack.top();
						command_stack.pop();
						state_current=State::NEWLINE;
						}
						break;
					
					case '\\':
						state_current=State::ESCAPE;
						break;
 
					default:
						token_in+=ch_in;
					}
				break;
				
			case State::ESCAPE:
				token_in+=ch_in;
				state_current=State::STATEMENT;
				break;
			
			case State::COMMENT:
				switch(ch_in)
					{
					case '\r':
					case '\n':
						state_current=State::NEWLINE;
						break;
					}
				break;
			}
		}
	}
