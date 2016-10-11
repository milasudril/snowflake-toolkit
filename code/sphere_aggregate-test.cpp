//@	{
//@	"targets":
//@		[{
//@		 "name":"sphere_aggregate-test","type":"application"
//@		,"cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@		}]
//@	}

#include "sphere_aggregate.h"
#include "grid.h"
#include "adda.h"
#include "file_out.h"

int main()
	{
	try
		{
		SnowflakeModel::SphereAggregate test;
		test.subvolumeAdd(SnowflakeModel::Sphere
			{SnowflakeModel::Point(0.0f,0.0f,0.0f,1.0f),1.0f});


		auto dV=test.volumeGet()/1000;
		auto dx=std::pow(dV,1.0/3.0);
		auto dy=std::pow(dV,1.0/3.0);
		auto dz=std::pow(dV,1.0/3.0);
		SnowflakeModel::Grid grid(dx,dy,dz,test.boundingBoxGet());
		test.geometrySample(grid);

		addaShapeWrite(grid,SnowflakeModel::FileOut("test.adda"));
		}
	catch(const char* message)
		{
		fprintf(stderr,"%s\n",message);
		}
	return 0;
	}