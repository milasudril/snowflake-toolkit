//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"snowflake_generate",
//@	            "type":"application"
//@			,"cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@	        }
//@	    ]
//@	}
#include "aggregate_graph.h"
#include "aggregate_graph_loader.h"
#include "config_parser.h"
#include "solid_builder_bbc.h"
#include "solid_writer.h"
#include "solid_writer_prototype.h"
#include "grid.h"
#include "adda.h"
#include "file_out.h"

#include <string>
#include <getopt.h>

struct Setup
	{
	std::string m_aggregate;
	std::string m_mesh_output;
	std::string m_geom_output;
	std::string m_mesh_ice_output;
	std::map<std::string,std::string> m_params;
	unsigned int size_x;
	unsigned int size_y;
	unsigned int size_z;
	bool help_show;
	bool param_show;

	Setup(int argc,char** argv);
	};

static const struct option PROGRAM_OPTIONS[]=
	{
		 {"aggregate",required_argument,nullptr,'a'}
		,{"help",no_argument,nullptr,'h'}
		,{"mesh-output",required_argument,nullptr,'m'}
		,{"mesh-output-ice",required_argument,nullptr,'M'}
		,{"sample-geometry",required_argument,nullptr,'s'}
		,{"param-show",no_argument,nullptr,'p'}
		,{0,0,0,0}
	};

Setup::Setup(int argc,char** argv):
	size_x(1),size_y(0),size_z(0),help_show(0),param_show(0)
	{
	int option_index;
	int c;
	const char* sample_geometry=nullptr;
	const char* aggregate=nullptr;
	while( (c=getopt_long(argc,argv,"",PROGRAM_OPTIONS,&option_index))!=-1)
		{
		switch(c)
			{
			case 'h':
				help_show=1;
				break;
			case 'a':
				aggregate=optarg;
				break;
			case 'm':
				m_mesh_output=optarg;
				break;
			case 's':
				sample_geometry=optarg;
				break;
			case 'p':
				param_show=1;
				break;
			case 'M':
				m_mesh_ice_output=optarg;
				break;
			case '?':
				throw "Invalid parameter given";
			}
		}
	if(aggregate==nullptr && !help_show)
		{
		throw "Aggregate description file is not given. "
			"Try --help for more information.";
		}

	if(aggregate!=nullptr)
		{
		int k=0;
		std::string temp;
		std::string param_name;
		while(*aggregate!='\0')
			{
			switch(*aggregate)
				{
				case ',':
					switch(k)
						{
						case 0:
							m_aggregate=temp;
							break;
						default:
							{
							auto ip=m_params.insert({param_name,temp});
							if(!ip.second)
								{throw "Aggregate parameter already given";}
							}
						}
					temp.clear();
					++k;
					break;
				case '=':
					switch(k)
						{
						case 0:
							temp+=*aggregate;
							break;
						default:
							param_name=temp;
							temp.clear();
						}
					break;
				default:
					temp+=*aggregate;
				}
			++aggregate;
			}
		switch(k)
			{
			case 0:
				m_aggregate=temp;
				break;
			default:
				{
				auto ip=m_params.insert({param_name,temp});
				if(!ip.second)
					{throw "Aggregate parameter already given";}
				}
			}
		if(m_aggregate.size()==0)
			{
			throw "No aggregate given";
			}
		}

	if(sample_geometry!=nullptr)
		{
		int k=0;
		std::string temp;
		while(*sample_geometry!='\0' && k!=3)
			{
			switch(*sample_geometry)
				{
				case ',':
					switch(k)
						{
						case 0:
							size_x=atoi(temp.data());
							break;
						case 1:
							size_y=atoi(temp.data());
							break;
						case 2:
							size_z=atoi(temp.data());
							break;
						}
					temp.clear();
					++k;
					break;
				default:
					temp+=*sample_geometry;
				}
			++sample_geometry;
			}
		if(*sample_geometry=='\0')
			{
			size_z=atoi(temp.data());
			m_geom_output="/dev/stdout";
			}
		else
			{m_geom_output=sample_geometry;}
		}
	}

int main(int argc,char** argv)
	{
	try
		{
		Setup setup(argc,argv);
		if(setup.help_show)
			{
			fprintf(stderr,"Options:\n\n"
				"--aggregate=aggregate_file[,parameters]\n"
				"    Generate data from the aggregates found in aggregate_file."
				" See the reference manal for information about how to create "
				"such a file.\n\n"
				"--help\n"
				"    Print this text and exit.\n\n"
				"--mesh-output=mesh_file\n"
				"    Export the output geometry as a Wavefront file, that can "
				"be imported into 3D modelling software such as Blender.\n\n"
				"--mesh-output-ice=mesh_file\n"
				"    Same as --dump-geometry but write data as an Ice crystal "
				"prototype file, so it can be used by other tools provided by "
				"the toolkit.\n\n"
				"--param-show\n"
				"    Print all availible parameters.\n\n"
				"--sample-geometry=Nx,Ny,Nz,adda_file\n"
				"    Sample the output geometry to a grid of size Nx x Ny x Nz"
				" voxels, and store the output to adda_file.\n"
				"    Nx, Ny, and Nz are integers. Up to two of these values can"
				" be zero can. Any missing value is computed to preserve one of"
				" two possible aspect ratios. If only one value is missnig, the"
				" given Nx is used in favour of Ny, which is used in favour of "
				"Nz.\n"
				"    If adda_file is omitted, data is written to /dev/stdout");
			return 0;
			}

		SnowflakeModel::AggregateGraph graph;
		std::map<std::string,SnowflakeModel::Solid> solids;
			{
			SnowflakeModel::FileIn src(setup.m_aggregate.data());
			SnowflakeModel::ConfigParser conf_parser(src);
			SnowflakeModel::AggregateGraphLoader loader(graph,setup.m_params,solids);
			conf_parser.commandsRead(loader);

			if(setup.param_show)
				{
				auto i=loader.varsBegin();
				fprintf(stderr,"\nParameters:\n");
				while(i!=loader.varsEnd())
					{
					fprintf(stderr,"    %s=%s\n",i->first.data(),i->second.data());
					++i;
					}
				}
			}



		SnowflakeModel::Solid m;
		graph.nodesVisit(SnowflakeModel::SolidBuilderBBC(m));

		m.centerCentroidAt(SnowflakeModel::Point{0,0,0,1});

		if(setup.m_mesh_output!="")
			{
			SnowflakeModel::FileOut file_out(setup.m_mesh_output.data());
			SnowflakeModel::SolidWriter writer(file_out);
			writer.write(m);
			}

		if(setup.m_mesh_ice_output!="")
			{
			SnowflakeModel::FileOut file_out(setup.m_mesh_ice_output.data());
			SnowflakeModel::SolidWriterPrototype writer(file_out);
			writer.write(m);
			}

		if(setup.m_geom_output!="")
			{
			SnowflakeModel::Grid grid(setup.size_x,setup.size_y,setup.size_z
				,m.boundingBoxGet());

			m.geometrySample(grid);

			addaShapeWrite(grid,SnowflakeModel::FileOut(setup.m_geom_output.data()));
			}
		}
	catch(const char* message)
		{
		fprintf(stderr,"Error: %s\n",message);
		return -1;
		}
	return 0;
	}
