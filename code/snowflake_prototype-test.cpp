//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"snowflake_prototype-test",
//@	            "type":"application"
//@	        }
//@	    ]
//@	}
#include "config_parser.h"
#include "solid_loader.h"
#include "solid.h"
#include "ice_particle.h"
#include "solid_writer.h"
#include "solid_writer_prototype.h"
#include "file_out.h"
#include "voxelbuilder_adda.h"

#include <getopt.h>
#include <vector>

static constexpr char PARAM_HELP='@';
static constexpr char PARAM_SHAPE='A';
static constexpr char PARAM_DEFORMATION='B';
static constexpr char PARAM_OUTFILE='C';
static constexpr char PARAM_PARAMSHOW='N';
static constexpr char PARAM_GEOMETRY_SAMPLE='D';
static constexpr char PARAM_OUTFILE_ICE='E';

static const struct option PROGRAM_OPTIONS[]=
	{
		 {"help",no_argument,nullptr,PARAM_HELP}
		,{"shape",required_argument,nullptr,PARAM_SHAPE}
		,{"deformation",required_argument,nullptr,PARAM_DEFORMATION}
		,{"output",required_argument,nullptr,PARAM_OUTFILE}
		,{"output-ice",required_argument,nullptr,PARAM_OUTFILE_ICE}
		,{"param-show",no_argument,nullptr,PARAM_PARAMSHOW}
		,{"sample-geometry",required_argument,nullptr,PARAM_GEOMETRY_SAMPLE}
		,{0,0,0,0}
	};

struct DeformationData
	{
	std::string name;
	double value;
	};

struct Setup
	{
	std::string m_crystal;
	std::vector<DeformationData> m_deformations;
	std::string m_output_obj;
	std::string m_output_ice;
	std::string m_geom_output;

	static constexpr uint32_t HELP_SHOW=1;
	static constexpr uint32_t PARAM_SHOW=16;

	uint32_t m_actions;
	int m_size_x;
	int m_size_y;
	int m_size_z;

	Setup(int argc,char** argv);
	void paramsDump();
	};

Setup::Setup(int argc,char** argv):m_actions(0)
	{
	int option_index;
	int c;
	std::vector<std::string> deformations;
	const char* sample_geometry=nullptr;


	while( (c=getopt_long(argc,argv,"",PROGRAM_OPTIONS,&option_index))!=-1)
		{
		switch(c)
			{
			case PARAM_HELP:
				m_actions|=HELP_SHOW;
				break;

			case PARAM_SHAPE:
				m_crystal=optarg;
				break;

			case PARAM_DEFORMATION:
			//	This option takes sub-options
				deformations.push_back(optarg);
				break;

			case PARAM_GEOMETRY_SAMPLE:
			//	This option takes sub-options.
				sample_geometry=optarg;
				break;

			case PARAM_PARAMSHOW:
				m_actions|=PARAM_SHOW;
				break;

			case PARAM_OUTFILE:
				m_output_obj=optarg;
				break;

			case PARAM_OUTFILE_ICE:
				m_output_ice=optarg;
				break;

			case '?':
				throw "Invalid parameter given";

			}
		}

	if(m_crystal=="" && !(m_actions&HELP_SHOW))
		{
		throw "Crystal file is not given. "
			"Try --help for more information.";
		}

	if(m_output_obj=="" && m_output_ice=="" && sample_geometry==nullptr
		&& !(  (m_actions&HELP_SHOW) || (m_actions&PARAM_SHOW) ))
		{
		throw "Output file is not given. "
			"Try --help for more information.";
		}

	if(deformations.size()==0 && !( (m_actions&HELP_SHOW) || (m_actions&PARAM_SHOW)))
		{
		throw "No deformation is given. "
			"Try --param-show together with the chosen crystal file for more "
			"information.";
		}

		{
		auto ptr=deformations.data();
		auto ptr_end=ptr+deformations.size();
		DeformationData deformation;
		deformation.value=1;
		while(ptr!=ptr_end)
			{
			auto strptr=ptr->data();
			auto str_end=strptr+ptr->size();
			std::string str_temp;
			size_t fieldcount=0;
			while(strptr!=str_end)
				{
				switch(*strptr)
					{
					case ',':
						switch(fieldcount)
							{
							case 0:
								deformation.name=str_temp;
								break;
							case 1:
								deformation.value=atof(str_temp.data());
								break;
							default:
								throw "Too many arguments for deformation";
							}
						++fieldcount;
						str_temp.clear();
						break;

					default:
						str_temp+=*strptr;
					}
				++strptr;
				}
			switch(fieldcount)
				{
				case 0:
					deformation.name=str_temp;
					break;
				case 1:
					deformation.value=atof(str_temp.data());
					break;
				}
			m_deformations.push_back(deformation);
			deformation.value=1;
			deformation.name.clear();
			++ptr;
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
							m_size_x=atoi(temp.data());
							break;
						case 1:
							m_size_y=atoi(temp.data());
							break;
						case 2:
							m_size_z=atoi(temp.data());
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
			m_size_z=atoi(temp.data());
			m_geom_output="/dev/stdout";
			}
		else
			{m_geom_output=sample_geometry;}
		}
	}

void Setup::paramsDump()
	{
	printf("Parameters:\n\n"
		"Shape:       %s\n"
		,m_crystal.data());
	printf("\nDeformations:\n");
		{
		auto ptr=m_deformations.data();
		auto ptr_end=ptr+m_deformations.size();
		while(ptr!=ptr_end)
			{
			printf("    name=%s, value=%.7g\n",ptr->name.data(),ptr->value);
			++ptr;
			}
		}
	}

void helpShow()
	{
	printf("Options:\n\n"
		"--shape=crystal_file\n"
		"    Generate data using the shape stored in crystal_file."
		" See the reference manal for information about how to create "
		"such a file.\n\n"
		"--deformation=name,value\n"
		"    Crystal file parameter to use for size modification. Use\n\n"
		"         --param-show\n\n"
		"    to list the name of available parameters.\n\n"
		"--output=output_file\n"
		"    Wavefront file to write transformed geometry to.\n\n"
		"--output-ice=output_file\n"
		"    Ice crystal prototype file to write transformed geometry to.\n\n"
		"--sample-geometry=Nx,Ny,Nz,filename\n"
		"    Sample the output geometry to a grid of size Nx x Ny x Nz\n\n"
		);
	}

int main(int argc,char** argv)
	{
	try
		{
		Setup setup(argc,argv);
		if(setup.m_actions&Setup::HELP_SHOW)
			{
			helpShow();
			return 0;
			}
		SnowflakeModel::Solid solid_in;
			{
			SnowflakeModel::FileIn file_in(setup.m_crystal.data());
			SnowflakeModel::ConfigParser parser(file_in);
			SnowflakeModel::SolidLoader loader(solid_in);
			parser.commandsRead(loader);
			}

		if(setup.m_actions&Setup::PARAM_SHOW)
			{
			auto& params=solid_in.deformationTemplatesGet();
			auto ptr=params.data();
			auto end=ptr+params.size();
			printf("Available parameters are:\n");
			while(ptr!=end)
				{
				auto paramname=ptr->paramnamesBegin();
				auto paramnames_end=ptr->paramnamesEnd();
				while(paramname!=paramnames_end)
					{
					printf("    %s\n",paramname->data());
					++paramname;
					}
				++ptr;
				}
			return 0;
			}

		setup.paramsDump();

		SnowflakeModel::IceParticle particle_out;
		particle_out.solidSet(solid_in);

			{
			auto ptr_param=setup.m_deformations.data();
			auto ptr_end=ptr_param+setup.m_deformations.size();
			while(ptr_param!=ptr_end)
				{
				particle_out.parameterSet(ptr_param->name,ptr_param->value);
				++ptr_param;
				}
			}

		if(setup.m_output_obj!="")
			{
			SnowflakeModel::FileOut file_out(setup.m_output_obj.data());
			SnowflakeModel::SolidWriter writer(file_out);
			writer.write(particle_out.solidGet());
			}

		if(setup.m_geom_output!="")
			{
			SnowflakeModel::FileOut file_out(setup.m_geom_output.data());
			SnowflakeModel::VoxelbuilderAdda builder(file_out
				,setup.m_size_x,setup.m_size_y,setup.m_size_z
				,particle_out.solidGet().boundingBoxGet());

			particle_out.solidGet().geometrySample(builder);
			}

		if(setup.m_output_ice!="")
			{
			SnowflakeModel::FileOut file_out(setup.m_output_ice.data());
			SnowflakeModel::SolidWriterPrototype writer(file_out);
			writer.write(particle_out.solidGet());
			}

		return 0;
		}
	catch(const char* message)
		{
		fprintf(stderr,"# Error: %s\n",message);
		return -1;
		}
	}
