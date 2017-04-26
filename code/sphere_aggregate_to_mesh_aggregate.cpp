//@	{
//@	"targets":
//@		[{
//@		 "name":"sphere_aggregate_to_mesh_aggregate","type":"application"
//@		,"description":"Converter from sphere aggregate to mesh aggregate"
//@		}]
//@	}

#include "sphere_aggregate.h"
#include "sphere_aggregate_loader.h"
#include "solid.h"
#include "solid_writer_prototype.h"
#include "filename.h"
#include "config_parser.h"
#include "file_out.h"
#include "alice/commandline.hpp"

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{
	 "Input options","source","The name of the source file. If this option is omitted, stdin is used."
	,"filename",Alice::Option::Multiplicity::ONE
	}
	,{
	 "Output options","subdivs","The number of subdivisions used when generating icospheres.","unsigned int"
	,Alice::Option::Multiplicity::ONE
	}
	,{
	 "Output options","dest","The name of the destination file. If filename is omitted, data is written to stdout. "
	,"filename"
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

static void aggregateStore(const SnowflakeModel::Solid& solid,const char* filename)
	{
	auto dest=filename==nullptr?
		 SnowflakeModel::FileOut(stdout)
		:SnowflakeModel::FileOut(filename);

	SnowflakeModel::SolidWriterPrototype writer(dest);
	writer.write(solid);
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

		auto aggregate_mesh=[&aggregate,&cmdline]()
			{
			auto& x=cmdline.get<Alice::Stringkey("subdivs")>();
			auto n=std::min(x.valueGet(),1u);
			return SnowflakeModel::Solid(aggregate,n);
			}();


		auto& x=cmdline.get<Alice::Stringkey("dest")>();
		aggregateStore(aggregate_mesh,x?x.valueGet().c_str():nullptr);
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
