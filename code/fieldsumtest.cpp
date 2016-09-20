//@	{"targets":[
//@	{"name":"fieldsumtest","type":"application"
//@		,"cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@	}]}

#include "fieldsum.h"
#include "solid_loader.h"
#include "config_parser.h"
#include "solid.h"
#include "ice_particle.h"
#include "profile.h"

int main()
	{
	try
		{
		SnowflakeModel::FileIn src("../crystal-prototypes/sphere-hollow.ice");
		SnowflakeModel::ConfigParser parser(src);
		SnowflakeModel::Solid prototype;
		SnowflakeModel::SolidLoader loader(prototype);
		parser.commandsRead(loader);

		SnowflakeModel::IceParticle p;
		p.solidSet(prototype);
		p.parameterSet("r_x",1.0f);
		p.parameterSet("r_y",1.0f);
		p.parameterSet("r_z",1.0f);

		auto& s=p.solidGet();
	//	Force computation of normals:
		auto v=s.volumeGet();
		printf("%.15g\n",v);
	
		double F=0;
			{
			SNOWFLAKEMODEL_TIMED_SCOPE();
			auto subvol=s.subvolumesBegin();
			auto N_tris=subvol->facesCount();
			while(N_tris!=0)
				{
				--N_tris;
				F+=flowIntegrate({0.0f,0.0f,0.0f,1.0f},subvol->triangleGet(N_tris));
				}
			}

		printf("Total flow: %.15g\n",F/(4.0*acos(-1.0f)));
		}
	catch(const char* message)
		{
		fprintf(stderr,"Error: %s\n",message);
		return -1;
		}
	return 0;
	}