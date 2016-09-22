//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"snowflake_simulate4",
//@	            "type":"application"
//@			,"cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@	        }
//@	    ]
//@	}
#include "solid_loader.h"
#include "file_in.h"
#include "file_out.h"
#include "solid_writer.h"
#include "config_parser.h"
#include "solid.h"
#include "ice_particle.h"
#include "randomgenerator.h"

static float sizeGet(SnowflakeModel::RandomGenerator& randgen,float mean,float std)
	{
//	Parameter setup from Wikipedia article.
//	Use the standard deviation from user input:
//		sigma^2=k*theta^2
//	we have
	float E=mean;
	float sigma=std;
	float k=E*E/(sigma*sigma);
	float theta=sigma*sigma/E;
//	Convert to C++ notation
	auto beta=theta;
	float alpha=k;

	std::gamma_distribution<float> G(alpha,beta);
	return G(randgen);
	}


static SnowflakeModel::Triangle
faceChoose(const SnowflakeModel::Solid& s_a,SnowflakeModel::RandomGenerator& randgen)
	{
	std::vector<float> weights;
	auto v_current=s_a.subvolumesBegin();
	while(v_current!=s_a.subvolumesEnd())
		{
		weights.push_back(v_current->areaVisibleGet());
		++v_current;
		}
	std::discrete_distribution<size_t> P_v(weights.begin(),weights.end());
	auto& subvol_sel=s_a.subvolumeGet(P_v(randgen));
	weights.clear();
	auto f_i=subvol_sel.facesOutBegin();
	while(f_i!=subvol_sel.facesOutEnd())
		{
		weights.push_back(glm::length(subvol_sel.faceGet(*f_i).m_normal_raw));
		++f_i;
		}
	std::discrete_distribution<size_t> P_f(weights.begin(),weights.end());
	auto face_out_index=P_f(randgen);
	return subvol_sel.triangleOutGet(face_out_index);
	}


int main()
	{
	try
		{
		SnowflakeModel::FileIn src("../crystal-prototypes/spheroid-lowpoly.ice");
		SnowflakeModel::ConfigParser parser(src);
		SnowflakeModel::Solid prototype;
		SnowflakeModel::SolidLoader loader(prototype);
		parser.commandsRead(loader);


		SnowflakeModel::IceParticle p;
		p.solidSet(prototype);
		p.parameterSet("r_x",1.0f);
		p.parameterSet("r_y",1.0f);
		p.parameterSet("r_z",1.0f);
	
		size_t N=2000;
		SnowflakeModel::Solid solid_out;
		solid_out.merge(p.solidGet(),0,0);
		SnowflakeModel::RandomGenerator randgen;
		fprintf(stderr,"# Running\n");
		fflush(stderr);
		while(N!=0)
			{
			fprintf(stderr,"\r%zu       ",N);
			fflush(stderr);
			p.parameterSet("s",sizeGet(randgen,1.0f,0.25f));
			auto T_a=faceChoose(solid_out,randgen);
			auto T_b=faceChoose(p.solidGet(),randgen);

			auto u=(T_a.vertexGet(0) + T_a.vertexGet(1) + T_a.vertexGet(2))/3.0f;
			auto v=(T_b.vertexGet(0) + T_b.vertexGet(1) + T_b.vertexGet(2))/3.0f;
			
			auto R=SnowflakeModel::vectorsAlign2(T_b.m_normal,-T_a.m_normal);
			auto pos=SnowflakeModel::Vector(u - R*v);

			SnowflakeModel::Matrix T;
			T=glm::translate(T,pos);
			auto obj=p.solidGet();
			obj.transform(T*R,0);
			if(!overlap(obj,solid_out))
				{
				solid_out.merge(obj,0,0);
				--N;
				}
			}

		





		SnowflakeModel::FileOut dest("test.obj");
		SnowflakeModel::SolidWriter writer(dest);
		writer.write(solid_out);

		


	/*	auto& solid_deformed=p.solidGet();
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
		p2.merge(solid_deformed,0,0);*/

		
		}
	catch(const char* msg)
		{
		fprintf(stderr,"%s\n",msg);
		return -1;
		}

	return 0;
	}
