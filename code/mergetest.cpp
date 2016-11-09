//@	{
//@	"dependencies_extra":[],
//@	"targets":
//@		[{
//@		"dependencies":[]
//@		,"name":"mergetest"
//@		,"type":"application"
//@		,"description":"Test application demonstrating how to merge particles"
//@		}]
//@	}
#include "solid_loader.h"
#include "file_in.h"
#include "file_out.h"
#include "solid_writer.h"
#include "config_parser.h"
#include "solid.h"
#include "ice_particle.h"

int main()
	{
	try
		{
		SnowflakeModel::FileIn src("../crystal-prototypes/bullet.ice");
		SnowflakeModel::ConfigParser parser(src);
		SnowflakeModel::Solid prototype;
		SnowflakeModel::SolidLoader loader(prototype);
		parser.commandsRead(loader);

		SnowflakeModel::IceParticle p;
		p.solidSet(prototype);
		p.parameterSet("t",0);
		p.parameterSet("L",2.0f);
		p.parameterSet("a",0.33f);

		auto& solid_deformed=p.solidGet();
		solid_deformed.centerCentroidAt(SnowflakeModel::Point(0,0,0,1.0f));
		auto& vc_a=solid_deformed.subvolumeGet(0);
		auto f_a=vc_a.triangleGet(0);
		auto u=f_a.vertexGet(0) + 0.25f*(f_a.vertexGet(1) - f_a.vertexGet(0))
			 + 0.25f*(f_a.vertexGet(2) - f_a.vertexGet(0));

		auto p2=solid_deformed;
		auto& vc_b=p2.subvolumeGet(0);
		auto f_b=vc_b.triangleGet(2);
		auto v=f_b.vertexGet(0) + 0.25f*(f_b.vertexGet(1) - f_b.vertexGet(0))
			 + 0.25f*(f_b.vertexGet(2) - f_b.vertexGet(0));

		auto R=SnowflakeModel::vectorsAlign2(f_b.m_normal, -f_a.m_normal);
		auto pos=SnowflakeModel::Vector(u - R*v);
		SnowflakeModel::Matrix T;
		T=glm::translate(T,pos);
		solid_deformed.transform(T*R,0);
		p2.merge(solid_deformed,0,0);

		SnowflakeModel::FileOut dest("test.obj");
		SnowflakeModel::SolidWriter writer(dest);
		writer.write(p2);
		}
	catch(const char* msg)
		{
		fprintf(stderr,"%s\n",msg);
		return -1;
		}

	return 0;
	}
