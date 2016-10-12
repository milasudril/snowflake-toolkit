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
#include "solid.h"
#include "solid_writer.h"

int main()
	{
	try
		{
		SnowflakeModel::SphereAggregate test;
		test.subvolumeAdd(SnowflakeModel::Sphere
			{SnowflakeModel::Point(0.0f,0.0f,0.0f,1.0f),1.0f});

		auto posnormal=test.shoot(SnowflakeModel::Point(10.0f,sqrt(1.0f/2.0f),0.0f,1.0f)
			,SnowflakeModel::Vector(-1.0f,0.0f,0.0f)
			,1.0f,0.0f);

		printf("Location: %.7g,%.7g,%.7g\n"
			"Normal: %.7g,%.7g,%.7g\n"
			,posnormal.first.x,posnormal.first.y,posnormal.first.z
			,posnormal.second.x,posnormal.second.y,posnormal.second.z);

		auto r_2=2.0f;
		SnowflakeModel::Sphere v_2{posnormal.first + r_2*SnowflakeModel::Point(posnormal.second,0.0f),r_2};
		if(overlap(test,v_2))
			{printf("Overlap\n");}
		test.subvolumeAdd(v_2);

			{
			auto dV=test.volumeGet()/2000;
			auto dx=std::pow(dV,1.0/3.0);
			auto dy=dx;
			auto dz=dx;
			SnowflakeModel::Grid grid(dx,dy,dz,test.boundingBoxGet());
			test.geometrySample(grid);
			addaShapeWrite(grid,SnowflakeModel::FileOut("test.adda"));
			}

			{
			SnowflakeModel::Solid s(test,3);
			SnowflakeModel::FileOut test_obj("test.obj");
			SnowflakeModel::SolidWriter writer(test_obj);
			writer.write(s);
			}

		}
	catch(const char* message)
		{
		fprintf(stderr,"%s\n",message);
		}
	return 0;
	}
