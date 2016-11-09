//@	{
//@	"targets":
//@		[{
//@		 "name":"ctrlctest","type":"application"
//@		,"description":"Test application for CtrlCHandler or SIGINT capture"
//@		}]
//@	}

#include "ctrlchandler.h"

int main()
	{
	SnowflakeModel::CtrlCHandler handler;

	while(!handler.captured())
		{
		}

	printf("Ctrl-C captured");

	return 0;
	}