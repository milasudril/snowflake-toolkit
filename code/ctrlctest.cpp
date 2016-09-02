//@	{"targets":[{"name":"ctrlctest","type":"application"}]}

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