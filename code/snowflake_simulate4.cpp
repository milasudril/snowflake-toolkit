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
#include "solid_writer_prototype.h"
#include "ctrlchandler.h"
#include "filenameesc.h"
#include "getdate.h"
#include "alice/commandline.hpp"

struct Deformation
	{
	std::string name;
	float mean;
	float standard_deviation;
	};

void print(const Deformation& deformation,FILE* dest)
	{
	fprintf(dest,"{\"name\":\"%s\",%.7g,%.7g}",deformation.name.c_str()
		,deformation.mean
		,deformation.standard_deviation);
	}

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("Deformation rule")>
		{
		typedef Deformation Type;
		};

	template<>
	Deformation make_value<Deformation>(const std::string& str)
		{
		Deformation ret{};
		auto ptr=str.data();
		size_t fieldcount=0;
		std::string buffer;
		while(1)
			{
			auto x=*ptr;
			switch(fieldcount)
				{
				case 0:
					switch(x)
						{
						case ':':
							++fieldcount;
							break;
						case '\0':
							{throw "Deformation rule has no arguments";}
						default:
							ret.name+=*ptr;
						}
					break;

				case 1:
					switch(x)
						{
						case ':':
							ret.mean=atof(buffer.c_str());
							buffer.clear();
							++fieldcount;
							break;
						case '\0':
							ret.mean=atof(buffer.c_str());
							return ret;
						default:
							buffer+=x;
						}
					break;

				case 2:
					switch(x)
						{
						case ':':
							throw "Too many arguments for deformation rule";
						case '\0':
							ret.standard_deviation=atof(buffer.c_str());
							return ret;
						default:
							buffer+=x;
						}
					break;
				default:
					throw "Too many arguments for deformation rule";
				}
			++ptr;
			}

		return Deformation{};
		}
	};

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","String",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{"Help","params-show","Print a summary of availible deformation parameters to stdout, or, if a filename is given, to that file","String",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{"Simulation parameters","prototype","Generate data using the given prototype","String",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","deformations","Defines all deformations to apply to the prototype. A deformation rule is written in the form parameter:mean:standard deviation","Deformation rule",Alice::Option::Multiplicity::ONE_OR_MORE}
	,{"Simulation parameters","D_max","Generate a graupel of diameter D_max. D_max is defined as the largest distance between two vertices.","Double",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","seed","Random seed mod 2^32","Integer",Alice::Option::Multiplicity::ONE}
	,{"Output options","dump-geometry","Specify output Wavefront file","String",Alice::Option::Multiplicity::ONE}
	,{"Output options","dump-geometry-ice","Same as --dump-geometry but write data as Ice crystal prototype files, so they can be used by other tools provided by the toolkit.","String",Alice::Option::Multiplicity::ONE}
	,{"Output options","dump-stats","Write statistics to the given file","String",Alice::Option::Multiplicity::ONE}
	,{"Other","statefile","Reload state from file","String",Alice::Option::Multiplicity::ONE}
	);

static bool printHelp(const Alice::CommandLine<OptionDescriptor>& cmd_line)
	{
	const auto& x=cmd_line.get<Alice::Stringkey("help")>();
	if(x)
		{
		const auto& val=x.valueGet();
		if(val.size()>0)
			{cmd_line.help(1,SnowflakeModel::FileOut(val[0].c_str()).handleGet());}
		else
			{cmd_line.help(1);}
		return 1;
		}
	return 0;
	}

static SnowflakeModel::Solid prototypeLoad(const Alice::CommandLine<OptionDescriptor>& cmd_line)
	{
	const auto& src_name=cmd_line.get<Alice::Stringkey("prototype")>();
	if(!src_name)
		{throw "No crystal prototype is given";}

	SnowflakeModel::FileIn src(src_name.valueGet().c_str());
	
	SnowflakeModel::ConfigParser parser(src);
	SnowflakeModel::Solid prototype;
	SnowflakeModel::SolidLoader loader(prototype);
	parser.commandsRead(loader);

	return prototype;
	}

static void paramsShow(const SnowflakeModel::Solid& s,SnowflakeModel::FileOut&& file)
	{
	auto& deformations=s.deformationTemplatesGet();
	auto ptr=deformations.data();
	auto ptr_end=ptr + deformations.size();
	file.printf("Availible parameters\n"
		"====================\n\n");
	while(ptr!=ptr_end)
		{
		auto& name=ptr->nameGet();
		file.printf("%s\n",name.c_str());
		auto N=name.size();
		while(N)
			{
			--N;
			file.putc('-');
			}
		file.putc('\n');
		auto paramname=ptr->paramnamesBegin();
		auto paramnames_end=ptr->paramnamesEnd();
		while(paramname!=paramnames_end)
			{
			printf(" * %s\n",paramname->data());
			++paramname;
			}
		file.putc('\n');
		++ptr;
		}
	}

static bool paramsShow(const Alice::CommandLine<OptionDescriptor>& cmd_line,const SnowflakeModel::Solid& s)
	{
	const auto& x=cmd_line.get<Alice::Stringkey("params-show")>();
	if(x)
		{
		const auto& val=x.valueGet();
		if(val.size()>0)
			{paramsShow(s,SnowflakeModel::FileOut(val[0].c_str()));}
		else
			{paramsShow(s,SnowflakeModel::FileOut(stdout));}
		return 1;
		}
	return 0;
	}



SnowflakeModel::IceParticle particleGenerate(const SnowflakeModel::Solid& s_in
	,const std::vector<Deformation>& deformations,SnowflakeModel::RandomGenerator& randgen)
	{
	SnowflakeModel::IceParticle ice_particle;
	ice_particle.solidSet(s_in);

//	Setup parameters
		{
		auto deformation=deformations.data();
		auto deformation_end=deformation+deformations.size();
		while(deformation!=deformation_end)
			{
		//	Test if parameter is deterministic
			if(fabs(deformation->standard_deviation)<1e-7)
				{ice_particle.parameterSet(deformation->name.data(),deformation->mean);}
			else
				{
			//	It is not. Setup gamma distribution
			//
			//	Parameter setup from Wikipedia article.
			//	Use the standard deviation from user input:
			//		sigma^2=k*theta^2
			//	we have
				float E=deformation->mean;
				float sigma=deformation->standard_deviation;
				float k=E*E/(sigma*sigma);
				float theta=sigma*sigma/E;
			//	Convert to C++ notation
				auto beta=theta;
				float alpha=k;

				std::gamma_distribution<float> G(alpha,beta);
				auto scale=G(randgen);
				ice_particle.parameterSet(deformation->name,scale);
				}
			++deformation;
			}
		}
	return std::move(ice_particle);
	}

static SnowflakeModel::Vector drawSphere(SnowflakeModel::RandomGenerator& randgen)
	{
	std::uniform_real_distribution<float> U(-1.0f,1.0f);
	float x_1;
	float x_2;
	float sq_sum;
	do
		{
		x_1=U(randgen);
		x_2=U(randgen);
		sq_sum=x_1*x_1 + x_2*x_2;
		}
	while(sq_sum >= 1.0f);
	auto r=sqrt(1 - sq_sum);
	return SnowflakeModel::Vector{2.0f*x_1*r,2.0f*x_2*r,1.0f - 2.0f*sq_sum};
	}

static std::pair<SnowflakeModel::Triangle,float>
faceChoose(const SnowflakeModel::Solid& s_a,SnowflakeModel::RandomGenerator& randgen)
	{
//	Construct a sphere from the bounding box
	auto& bb=s_a.boundingBoxGet();
	auto bb_size=bb.sizeGet();
	auto bb_mid=bb.centerGet();
	auto r=0.5f*glm::length( bb_size );

	auto direction=drawSphere(randgen);


//	Set the source on the sphere
	auto source=bb_mid - SnowflakeModel::Point(r*direction,1.0);

	return s_a.shoot(source,direction);
	}

static void statsDump(SnowflakeModel::FileOut* file_out,const SnowflakeModel::Solid& solid)
	{
	if(file_out!=nullptr)
		{
		auto& bb=solid.boundingBoxGet();
		auto extrema=solid.extremaGet();
		auto L=bb.m_max-bb.m_min;
		fflush(stdout);
		file_out->printf("%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%zu\t"
			"%zu\n"
			,solid.rMaxGet()
			,glm::distance(extrema.first,extrema.second)
			,solid.volumeGet()
			,L.x,L.x/L.y,L.x/L.z,solid.subvolumesCount(),solid.overlapCount());
		}
	}

static std::unique_ptr<SnowflakeModel::FileOut> statFileOpen(const Alice::CommandLine<OptionDescriptor>& cmd_line)
	{
	const auto& x=cmd_line.get<Alice::Stringkey("dump-stats")>();
	if(x)
		{
		auto ret=std::make_unique<SnowflakeModel::FileOut>(x.valueGet().c_str());

		ret->printf("# R_max\tD_max\tVolume\tL_x\tr_xy\tr_xz\tNumber of sub-volumes\tOverlap count\n");

		return std::move(ret);
		}
	return std::unique_ptr<SnowflakeModel::FileOut>();
	}

static std::string statefileName(const std::string& name_init,const std::string& name_new)
	{
	if(name_init.size()==0)
		{
		return SnowflakeModel::filenameEscape(name_new.c_str()) + "-0000000000000000.h5";
		}
//TODO (perf) This can be done much easier by looping through name_init backwards
	auto pos_counter=name_init.find_last_of('-');
	auto ret=name_init.substr(0,pos_counter);
	auto pos_extension=name_init.find_last_of('.');
	auto val=name_init.substr(pos_counter + 1,pos_extension - pos_counter-1);
	size_t counter=strtol(val.c_str(),nullptr,16) + 1;
	sprintf(const_cast<char*>( val.data() ),"%016zx",counter);
	auto extension=name_init.substr(pos_extension);	
	return ret+"-"+val+extension;
	}


int main(int argc,char** argv)
	{
	try
		{
		Alice::CommandLine<OptionDescriptor> cmd_line(argc,argv);
		if(printHelp(cmd_line))
			{return 0;}

		auto prototype=prototypeLoad(cmd_line);

		if(paramsShow(cmd_line,prototype))
			{return 0;}

		const auto& deformations=cmd_line.get<Alice::Stringkey("deformations")>();
		if(!deformations)
			{throw "No deformation is given. Use --params-show to list the name of availible parameters";}

		cmd_line.print();

		auto D_max=cmd_line.get<Alice::Stringkey("D_max")>().valueGet();
		auto file_out=statFileOpen(cmd_line);

		SnowflakeModel::RandomGenerator randgen;
		randgen.seed(cmd_line.get<Alice::Stringkey("seed")>().valueGet());
		randgen.discard(65536);

		SnowflakeModel::Solid solid_out;
		auto p=particleGenerate(prototype,deformations.valueGet(),randgen);	
		solid_out.merge(p.solidGet(),0,0);
		auto ex=solid_out.extremaGet();
		auto d_max=length(ex.first - ex.second);
		fprintf(stderr,"# Running\n");
		fflush(stderr);
		size_t rejected=0;
		SnowflakeModel::CtrlCHandler int_handler;
		auto now=SnowflakeModel::getdate();
		do
			{
			p=particleGenerate(prototype,deformations.valueGet(),randgen);
			auto T_a=faceChoose(solid_out,randgen);
			if(T_a.second==INFINITY)
				{continue;}
			auto T_b=faceChoose(p.solidGet(),randgen);
			if(T_b.second==INFINITY)
				{continue;}


			auto u=(T_a.first.vertexGet(0) + T_a.first.vertexGet(1) + T_a.first.vertexGet(2))/3.0f;
			auto v=(T_b.first.vertexGet(0) + T_b.first.vertexGet(1) + T_b.first.vertexGet(2))/3.0f;
			
			auto R=SnowflakeModel::vectorsAlign2(T_b.first.m_normal,-T_a.first.m_normal);
			auto pos=SnowflakeModel::Vector(u - R*v);

			SnowflakeModel::Matrix T;
			T=glm::translate(T,pos);
			auto& obj=p.solidGet();
			obj.transform(T*R,0);
			if(!overlap(obj,solid_out))
				{
				solid_out.merge(std::move(obj),0,0);
				auto ex=solid_out.extremaGet();
				d_max=length(ex.first - ex.second);
				fprintf(stderr,"\r%.7g (Cs: %zu,  Cr: %zu)      "
					,d_max,solid_out.subvolumesCount(),rejected);
				statsDump(file_out.get(),solid_out);
				fflush(stderr);
				}
			else
				{++rejected;}
			}
		while(d_max < D_max && !int_handler.captured());

			{
			auto statefile=cmd_line.get<Alice::Stringkey("statefile")>().valueGet();
			auto filename_dump=statefileName(statefile,now);
			fprintf(stderr,"# Dumping simulation state to %s\n",filename_dump.c_str());
			SnowflakeModel::DataDump dump(filename_dump.c_str()
				,SnowflakeModel::DataDump::IOMode::WRITE);
		//	setup.write(dump);
			prototype.write("prototype",dump);
			solid_out.write("solid_out",dump);
			auto& rng_state=SnowflakeModel::get(randgen);
			dump.write("randgen_state",rng_state.state,1);
			dump.write("randgen_position",&rng_state.position,1);
			dump.write("D_max",&D_max,1);
				{
				auto& x=cmd_line.get<Alice::Stringkey("dump-stats")>();
				if(x)
					{
					auto y=x.valueGet().c_str();
					dump.write("statfile",&y,1);
					}
				}
				{
				auto x=cmd_line.get<Alice::Stringkey("prototype")>().valueGet().c_str();
				dump.write("prototype_source",&x,1);
				}
			}

			{
			fprintf(stderr,"# Dumping wavefront files\n");
			auto& objfile=cmd_line.get<Alice::Stringkey("dump-geometry")>();
			if(objfile)
				{
				SnowflakeModel::FileOut dest(objfile.valueGet().c_str());
				SnowflakeModel::SolidWriter writer(dest);
				writer.write(solid_out);
				}
			}

			{
			fprintf(stderr,"# Dumping crystal prototype files\n");
			auto& icefile=cmd_line.get<Alice::Stringkey("dump-geometry-ice")>();
			if(icefile)
				{
				SnowflakeModel::FileOut file_out(icefile.valueGet().c_str());
				SnowflakeModel::SolidWriterPrototype writer(file_out);
				writer.write(solid_out);
				}
			}
		}
	catch(const Alice::ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.data);
		return -1;
		}
	catch(const char* msg)
		{
		fprintf(stderr,"%s\n",msg);
		return -1;
		}

	return 0;
	}
