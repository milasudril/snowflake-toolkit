//@	{
//@	"targets":
//@		[{
//@		 "name":"sphere_aggregate_rasterize","type":"application"
//@		,"description":"Rasterizer for sphere aggregates"
//@		}]
//@	}

#include "sphere_aggregate.h"
#include "sphere_aggregate_loader.h"
#include "filename.h"
#include "config_parser.h"
#include "grid_definition.h"
#include "grid_definition2.h"
#include "file_out.h"
#include "grid.h"
#include "adda.h"
#include "alice/commandline.hpp"

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{
	 "Input options","source","The name of the source file. If this option is omitted, stdin is used."
	,"filename",Alice::Option::Multiplicity::ONE
	}
	,{
	 "Output options","grid","Specification of the rasterization grid. If filename is omitted, data is written to stdout. "
	 "See *Common types* for more information.","grid definition"
	,Alice::Option::Multiplicity::ONE
	}
	,{
	 "Output options","grid2","Specification of the rasterization grid. If filename is omitted, data is written to stdout. "
	 "See *Common types* for more information.","grid definition 2"
	,Alice::Option::Multiplicity::ONE
	}
	);

static void helpPrint(const Alice::CommandLine<OptionDescriptor>& options
	,const std::vector<std::string>& dest)
	{
	if(dest.size()==0)
		{options.help(1,stdout);}
	else
		{
		SnowflakeModel::FileOut output(dest[0].c_str());
		options.help(1,output.handleGet());
		}
	}

static SnowflakeModel::SphereAggregate aggregateLoad(const char* filename)
	{
	auto src=filename==nullptr?
		SnowflakeModel::FileIn(stdin):SnowflakeModel::FileIn(filename);

	SnowflakeModel::ConfigParser parser(src);
	SnowflakeModel::SphereAggregate ret;
	SnowflakeModel::SphereAggregateLoader loader(ret);
	parser.commandsRead(loader);

	return std::move(ret);
	}

static void geometrySample(const SnowflakeModel::SphereAggregate& solid
	,const SnowflakeModel::GridDefinition& grid)
	{
	SnowflakeModel::Grid grid_out(grid.N_x,grid.N_y,grid.N_z
		,solid.boundingBoxGet());
	solid.geometrySample(grid_out);
	auto dest=grid.filename.size()==0?
		SnowflakeModel::FileOut(stdout):SnowflakeModel::FileOut(grid.filename.c_str());
	addaShapeWrite(grid_out,std::move(dest));
	}

static void geometrySample(const SnowflakeModel::SphereAggregate& solid
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
		
		auto aggregate=[&cmdline]()
			{
			auto& x=cmdline.get<Alice::Stringkey("source")>();
			return aggregateLoad(x?x.valueGet().c_str():nullptr);
			}();

			{
			auto& x=cmdline.get<Alice::Stringkey("grid")>();
			if(x)
				{geometrySample(aggregate,x.valueGet());}
			}

			{
			auto& x=cmdline.get<Alice::Stringkey("grid2")>();
			if(x)
				{geometrySample(aggregate,x.valueGet());}
			}
		}
	catch(const Alice::ErrorMessage& msg)
		{
		fprintf(stderr,"Command line error: %s\n",msg.data);
		return -1;
		}
	catch(const char* msg)
		{
		fprintf(stderr,"Error: %s\n",msg);
		return -1;
		}
	return 0;
	}
