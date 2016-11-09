//@	{
//@	"targets":
//@		[{
//@		 "name":"prototypechoices_test","type":"application"
//@		,"description":"Test application demonstrating how to load, and store PrototypeChoices"
//@		}]
//@	}

#include "prototypechoices.h"
#include "resourceobject.h"
#include "file_in.h"
#include "solid.h"

int main()
	{
	try
		{
			{
			SnowflakeModel::PrototypeChoices choices("prototypeexample.json");
			SnowflakeModel::DataDump dd("test.h5",SnowflakeModel::DataDump::IOMode::WRITE);
			choices.write("choices",dd);
			}

			{
			SnowflakeModel::DataDump dd("test.h5",SnowflakeModel::DataDump::IOMode::READ);
			SnowflakeModel::PrototypeChoices choices(dd,"choices");

			SnowflakeModel::DataDump dd2("test2.h5",SnowflakeModel::DataDump::IOMode::WRITE);
			choices.write("choices",dd2);
			}
		}
	catch(const char* msg)
		{fprintf(stderr,"Error: %s\n",msg);}
	return 0;
	}
