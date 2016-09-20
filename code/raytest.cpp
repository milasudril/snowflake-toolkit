//@	{"targets":[
//@	{"name":"raytest","type":"application"
//@		,"cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@	}]}

#include "triangle.h"

int main()
	{
	SnowflakeModel::Triangle T
		{
		 {{-1.0f,-1.0f,1.0f,1.0f},{1.0f,-1.0f,1.0f,1.0f},{0.0f,1.0f,1.0f,1.0f}}
		};

	float pos;
	if(SnowflakeModel::intersects(T,{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,1.0f},pos))
		{printf("pos=%.7g\n",pos);}

	return 0;
	}