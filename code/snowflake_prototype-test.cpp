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
#include "grid.h"
#include "ice_particle.h"
#include "solid_writer.h"
#include "solid_writer_prototype.h"
#include "file_out.h"
#include "grid_definition.h"
#include "grid_definition2.h"
#include "adda.h"
#include "filename.h"
#include "profile.h"
#include "alice/commandline.hpp"

struct DeformationRule
	{
	std::string name;
	double value;
	};

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("deformation rule")>:public MakeTypeBase
		{
		typedef DeformationRule Type;
		static constexpr const char* descriptionShortGet()
			{return "name:value";}
		};

	template<class ErrorHandler>
	struct MakeValue<DeformationRule,ErrorHandler>
		{
		static DeformationRule make_value(const std::string& str);
		};

	template<class ErrorHandler>
	DeformationRule
	MakeValue<DeformationRule,ErrorHandler>::make_value(const std::string& str)
		{
		auto ptr=str.c_str();
		DeformationRule ret;
		enum class State:int{NAME,VALUE};
		auto state_current=State::NAME;
		std::string buffer;
		while(1)
			{
			auto ch_in=*ptr;
			switch(state_current)
				{
				case State::NAME:
					switch(ch_in)
						{
						case '=':
						case ',':
						case ':':
							state_current=State::VALUE;
							break;
						case '\0':
							throw "No value given for parameter";
						default:
							ret.name+=ch_in;
						}
					break;
				case State::VALUE:
					switch(ch_in)
						{
						case '\0':
							ret.value=Alice::make_value<double,ErrorHandler>(buffer);
							return ret;
						default:
							buffer+=ch_in;
						}
					break;
				}
			++ptr;
			}
		}
	}

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{
	  "Information","help","Print usage information to stdout, or optionally to filename"
	 ,"filename",Alice::Option::Multiplicity::ZERO_OR_ONE
	 }
	,{
	  "Information","params-show","Print availible parameters for the loaded prototype to stdout, or optionally to filename"
	 ,"filename",Alice::Option::Multiplicity::ZERO_OR_ONE
	 }
	,{
	  "Input options","prototype","Load crystal prototype from filename","filename"
	 ,Alice::Option::Multiplicity::ONE
	 }
	,{
	 "Input options","deformations"
	,"Set deformation rules for the loaded prototype. A deformation rule is written on the form [name,value]. "
	 "Use --params-show to see availible parameters."
	,"deformation rule",Alice::Option::Multiplicity::ZERO_OR_MORE
	 }
	,{
	 "Output options","dump-geometry","Dumps the rendered geometry in Wavefront format to stdout, or optionally to filename"
	,"filename",Alice::Option::Multiplicity::ZERO_OR_ONE
	 }
	,{
	 "Output options","dump-geometry-ice","Dumps the rendered geometry in ice format to stdout, or optionally to filename"
	,"filename",Alice::Option::Multiplicity::ZERO_OR_ONE
	 }
	,{
	 "Output options","dump-geometry-simple","Dumps the rendenred geometry in a simple line based format that is easy to "
	 "pasrse in MATLAB or Octave","filename",Alice::Option::Multiplicity::ZERO_OR_ONE
	 }
	,{
	 "Output options","sample-geometry","Samples the rendered geometry to a grid. If Filename is omitted, data is written to stdout. "
		"See *Common types* for more information."
	,"grid definition",Alice::Option::Multiplicity::ONE
	 }
	,{
	 "Output options","sample-geometry-2","Samples the rendered geometry to a grid. "
		"If filename is ommited, the output is written to stdout. "
		"See *Common types* for more information."			
	,"grid definition 2",Alice::Option::Multiplicity::ONE
	 }
	);

static void helpPrint(const Alice::CommandLine<OptionDescriptor>& cmdline
	,const std::vector<std::string>& dests)
	{
	if(dests.size()==0)
		{cmdline.help(1);}
	else
		{
		SnowflakeModel::FileOut output(dests[0].c_str());
		cmdline.help(1,output.handleGet());
		}
	}

static SnowflakeModel::Solid solidLoad(const std::string& prototype)
	{
	if(prototype.size()==0)
		{throw "No crystal prototype is given";}

	SnowflakeModel::Solid solid_in;
		{
		SNOWFLAKEMODEL_TIMED_SCOPE();
		SnowflakeModel::FileIn file_in(prototype.c_str());
		SnowflakeModel::ConfigParser parser(file_in);
		SnowflakeModel::SolidLoader loader(solid_in);
		parser.commandsRead(loader);
		}

	return std::move(solid_in);
	}


static void paramsShow(const SnowflakeModel::Solid& solid_in
	,const std::vector<std::string>& filename)
	{
	auto dest=filename.size()==0?
		SnowflakeModel::FileOut(stdout):SnowflakeModel::FileOut(filename[0].c_str());

	auto& params=solid_in.deformationTemplatesGet();
	auto ptr=params.data();
	auto end=ptr+params.size();
	dest.printf("Available parameters\n");
	dest.printf("--------------------\n");
	while(ptr!=end)
		{
		dest.printf("\n%s:\n\n",ptr->nameGet().c_str());
		auto paramname=ptr->paramnamesBegin();
		auto paramnames_end=ptr->paramnamesEnd();
		while(paramname!=paramnames_end)
			{
			dest.printf(" * %s\n",paramname->data());
			++paramname;
			}
		++ptr;
		}
	}

static void deformationsPrint(const std::vector<DeformationRule>& deformations)
	{
	auto ptr=deformations.data();
	auto ptr_end=ptr + deformations.size();
	printf("# Deformations\n");
	while(ptr!=ptr_end)
		{
		printf("#   %s=%.7g\n",ptr->name.c_str(),ptr->value);
		++ptr;
		}
	}

static SnowflakeModel::IceParticle
particleGenerate(const SnowflakeModel::Solid& solid_in
	,const std::vector<DeformationRule>& deformations)
	{
	SnowflakeModel::IceParticle particle_out;
	particle_out.solidSet(solid_in);
	auto ptr_param=deformations.data();
	auto ptr_end=ptr_param + deformations.size();
	while(ptr_param!=ptr_end)
		{
		particle_out.parameterSet(ptr_param->name,ptr_param->value);
		++ptr_param;
		}
	return std::move(particle_out);
	}

static void geometryDumpObj(const SnowflakeModel::Solid& solid
	,const std::vector<std::string>& filename)
	{
	auto dest=filename.size()==0?
		SnowflakeModel::FileOut(stdout):SnowflakeModel::FileOut(filename[0].c_str());
	SnowflakeModel::SolidWriter writer(dest);
	writer.write(solid);
	}

static void geometryDumpIce(const SnowflakeModel::Solid& solid
	,const std::vector<std::string>& filename)
	{
	auto dest=filename.size()==0?
		SnowflakeModel::FileOut(stdout):SnowflakeModel::FileOut(filename[0].c_str());
	SnowflakeModel::SolidWriterPrototype writer(dest);
	writer.write(solid);
	}

static void geometryDumpSimple(const SnowflakeModel::Solid& solid
	,const std::vector<std::string>& filename)
	{
	auto dest=filename.size()==0?
		SnowflakeModel::FileOut(stdout):SnowflakeModel::FileOut(filename[0].c_str());
	auto subvol=solid.subvolumesBegin();
	auto subvols_end=solid.subvolumesEnd();
	SNOWFLAKEMODEL_TIMED_SCOPE();
	size_t volcount=0;
	while(subvol!=subvols_end)
		{
		auto vert=subvol->verticesBegin();
		auto verts_end=subvol->verticesEnd();
		while(vert!=verts_end)
			{
			dest.printf("%zu 0 %.8g %.8g %.8g\n",volcount,vert->x,vert->y,vert->z);
			++vert;
			}
		auto face=subvol->facesBegin();
		auto faces_end=subvol->facesEnd();
		while(face!=faces_end)
			{
			dest.printf("%zu 1 %d %d %d\n",volcount,face->vertexGet(0),face->vertexGet(1),face->vertexGet(2));
			++face;
			}
		++subvol;
		++volcount;
		}
	}

static void geometrySample(const SnowflakeModel::Solid& solid
	,const SnowflakeModel::GridDefinition& grid)
	{
	SnowflakeModel::Grid grid_out(grid.N_x,grid.N_y,grid.N_z
		,solid.boundingBoxGet());
	solid.geometrySample(grid_out);
	auto dest=grid.filename.size()==0?
		SnowflakeModel::FileOut(stdout):SnowflakeModel::FileOut(grid.filename.c_str());
	addaShapeWrite(grid_out,std::move(dest));
	}

static void geometrySample(const SnowflakeModel::Solid& solid
	,const SnowflakeModel::GridDefinition2& grid)
	{
	auto scale=grid.r_x*grid.r_y*grid.r_z;

	auto dV=solid.volumeGet()/grid.N;

	auto dx=grid.r_x*std::pow(dV/scale,1.0/3.0);
	auto dy=grid.r_y*std::pow(dV/scale,1.0/3.0);
	auto dz=grid.r_z*std::pow(dV/scale,1.0/3.0);

	SnowflakeModel::Grid grid_out(dx,dy,dz,solid.boundingBoxGet());
	solid.geometrySample(grid_out);

	auto dest=grid.filename.size()==0?
		 SnowflakeModel::FileOut(stdout)
		:SnowflakeModel::FileOut(grid.filename.c_str());
	addaShapeWrite(grid_out,std::move(dest));
	}

int main(int argc,char** argv)
	{
	try
		{
		Alice::CommandLine<OptionDescriptor> cmdline(argc,argv);
			{
			auto& x=cmdline.get<Alice::Stringkey("help")>();
			if(x)
				{
				helpPrint(cmdline,x.valueGet());
				return 0;
				}
			}

		auto solid_in=solidLoad(cmdline.get<Alice::Stringkey("prototype")>().valueGet());
			{
			auto& x=cmdline.get<Alice::Stringkey("params-show")>();
			if(x)
				{
				paramsShow(solid_in,x.valueGet());
				return 0;
				}
			}

		const auto& deformations=cmdline.get<Alice::Stringkey("deformations")>().valueGet();
		deformationsPrint(deformations);

		auto particle_out=particleGenerate(solid_in,deformations);

			{
			auto& x=cmdline.get<Alice::Stringkey("dump-geometry")>();
			if(x)
				{geometryDumpObj(particle_out.solidGet(),x.valueGet());}
			}

			{
			auto& x=cmdline.get<Alice::Stringkey("dump-geometry-ice")>();
			if(x)
				{geometryDumpIce(particle_out.solidGet(),x.valueGet());}
			}

			{
			auto& x=cmdline.get<Alice::Stringkey("dump-geometry-simple")>();
			if(x)
				{geometryDumpSimple(particle_out.solidGet(),x.valueGet());}
			}

			{
			auto& x=cmdline.get<Alice::Stringkey("sample-geometry")>();
			if(x)
				{geometrySample(particle_out.solidGet(),x.valueGet());}
			}

			{
			auto& x=cmdline.get<Alice::Stringkey("sample-geometry-2")>();
			if(x)
				{geometrySample(particle_out.solidGet(),x.valueGet());}
			}

		return 0;
		}
	catch(const Alice::ErrorMessage& msg)
		{
		fprintf(stderr,"Error: %s\n",msg.data);
		return -1;
		}
	catch(const char* message)
		{
		fprintf(stderr,"# Error: %s\n",message);
		return -1;
		}
	}
