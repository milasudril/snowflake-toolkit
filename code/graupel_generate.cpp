//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"graupel_generate",
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
	struct MakeType<Stringkey("Deformation rule")>:public MakeTypeBase
		{
		typedef Deformation Type;
		static constexpr const char* descriptionShortGet() noexcept
			{return "parameter:mean,standard deviation";}
		};

	template<class ErrorHandler>
	struct MakeValue<Deformation,ErrorHandler>
		{
		static Deformation make_value(const std::string& str);
		};

	template<class ErrorHandler>
	Deformation MakeValue<Deformation,ErrorHandler>::make_value(const std::string& str)
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
						case ',':
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
						case ',':
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



namespace Alice
	{
	template<>
	struct MakeType<Stringkey("filename")>:public MakeType<Stringkey("string")>
		{
		typedef std::string Type;
		static constexpr const char* descriptionGet() noexcept
			{return "A valid filename";}
		};
	}

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{"Help","params-show","Print a summary of availible deformation parameters to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{"Simulation parameters","prototype","Generate data using the given prototype","filename",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","deformations","Defines all deformations to apply to the prototype.","Deformation rule",Alice::Option::Multiplicity::ONE_OR_MORE}
	,{"Simulation parameters","E_0","The maximal initial particle energe. The energy is chosen as U(0, E_0)","double",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","decay_distance","The number of units before the energy has decayed to 1/e","double",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","D_max","Generate a graupel of diameter D_max. D_max is defined as the largest distance between two vertices.","double",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","seed","Random seed mod 2^32","unsigned int",Alice::Option::Multiplicity::ONE}
	,{"Simulation parameters","fill-ratio","Set minimum fill ratio of the bounding sphere. This option is used when D_max has been reached, to increase the total mass. "
		 "If the fill ratio is non-zero, and D_max is fullfilled, only events that do not increase D_max are accepted.","double"
		,Alice::Option::Multiplicity::ONE}
	,{"Output options","dump-geometry","Specify output Wavefront file","filename",Alice::Option::Multiplicity::ONE}
	,{"Output options","dump-geometry-ice","Same as --dump-geometry but write data as Ice crystal prototype files, so they can be used by other tools provided by the toolkit.","filename",Alice::Option::Multiplicity::ONE}
	,{"Output options","dump-stats","Write statistics to the given file","filename",Alice::Option::Multiplicity::ONE}
	,{"Other","statefile","Reload state from file","filename",Alice::Option::Multiplicity::ONE}
	);


struct DeformationDataOut
	{
	const char* name;
	float mean;
	float standard_deviation;
	};

struct DeformationDataIn
	{
	SnowflakeModel::DataDump::StringHolder name;
	float mean;
	float standard_deviation;
	};

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<DeformationDataOut>::fields[]=
		{
		 {"name",offsetOf(&DeformationDataOut::name),DataDump::MetaObject<decltype(DeformationDataOut::name)>().typeGet()}
		,{"mean",offsetOf(&DeformationDataOut::mean),DataDump::MetaObject<decltype(DeformationDataOut::mean)>().typeGet()}
		,{"standard_deviation",offsetOf(&DeformationDataOut::standard_deviation),DataDump::MetaObject<decltype(DeformationDataOut::standard_deviation)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<DeformationDataOut>::field_count=3;

	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<DeformationDataIn>::fields[]=
		{
		 {"name",offsetOf(&DeformationDataIn::name),DataDump::MetaObject<decltype(DeformationDataIn::name)>().typeGet()}
		,{"mean",offsetOf(&DeformationDataIn::mean),DataDump::MetaObject<decltype(DeformationDataIn::mean)>().typeGet()}
		,{"standard_deviation",offsetOf(&DeformationDataIn::standard_deviation),DataDump::MetaObject<decltype(DeformationDataIn::standard_deviation)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<DeformationDataIn>::field_count=3;
	}

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
	std::uniform_real_distribution<float> xi_dist(0,2.0f*std::acos(-1.0f));
	std::uniform_real_distribution<float> eta_dist(-1.0f,1.0f);
	
	auto xi=xi_dist(randgen);
	auto eta=eta_dist(randgen);
	auto r=std::sqrt(1.0f - eta*eta);
	return SnowflakeModel::Vector
		{
		 std::cos(xi)*r
		,std::sin(xi)*r
		,-eta
		};
	}

static std::pair<SnowflakeModel::Triangle,float>
faceChoose(const SnowflakeModel::Solid& s_a,SnowflakeModel::RandomGenerator& randgen
	,float E_0,float decay_distance,bool backface_culling)
	{
//	Construct a sphere from the bounding box
	auto& bb=s_a.boundingBoxGet();
	auto bb_size=bb.sizeGet();
	auto bb_mid=bb.centerGet();
	auto r=0.5f*glm::length( bb_size );

	auto direction=drawSphere(randgen);


//	Set the source on the sphere
	auto source=bb_mid - SnowflakeModel::Point(r*direction,1.0);

	return s_a.shoot(source,direction,E_0,decay_distance,backface_culling);
	}


static SnowflakeModel::Point drawBall(SnowflakeModel::RandomGenerator& randgen
	,const SnowflakeModel::Point& mid,float r)
	{
	std::uniform_real_distribution<float> X(mid.x - r,mid.x + r);
	std::uniform_real_distribution<float> Y(mid.x - r,mid.y + r);
	std::uniform_real_distribution<float> Z(mid.z - r,mid.z + r);

	SnowflakeModel::Point ret;
	do
		{
		ret={X(randgen),Y(randgen),Z(randgen),1.0f};
		}
	while(glm::distance(ret,mid)>=r);
	return ret;
	}

static std::pair<SnowflakeModel::Triangle,float>
faceChoose2(const SnowflakeModel::Solid& s_a,SnowflakeModel::RandomGenerator& randgen
	,float E_0,float decay_distance,bool backface_culling)
	{
//	Construct a sphere from the bounding box
	auto& bb=s_a.boundingBoxGet();
	auto bb_size=bb.sizeGet();
	auto bb_mid=bb.centerGet();
	auto r=0.5f*glm::length( bb_size );

	auto direction=drawSphere(randgen);


//	Set the source on the sphere
	auto source=bb_mid - SnowflakeModel::Point(r*direction,1.0);

//	Set the target somewhere inside the ball
	auto target=drawBall(randgen,bb_mid,r);

//	Compute a new direction
	direction=SnowflakeModel::Vector(target-source);

	return s_a.shoot(source,direction/glm::length(direction),E_0,decay_distance,backface_culling);
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

static std::string statefileName(const std::string& name_init,const std::string& name_new)
	{
	if(name_init.size()==0)
		{
		return std::string("graupel-") 
			+ SnowflakeModel::filenameEscape(name_new.c_str())
			+ "-0000000000000000.h5";
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

struct Simstate
	{
	Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line):r_cmd_line(cmd_line)
		{}

	explicit Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line
		,SnowflakeModel::Solid&& r_prototype);
	
	explicit Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line
		,const std::string& statefile);

	double progressGet() const noexcept
		{
		auto x=d_max/D_max;
		if(x<1.0 || fill_ratio<1e-2f)
			{return x;}
		return fill/fill_ratio;
		}

	void step();

	void save(const std::string& now) const;

	void objfileWrite() const;

	void icefileWrite() const;
	
	const Alice::CommandLine<OptionDescriptor>& r_cmd_line;
	SnowflakeModel::Solid m_prototype;
	std::vector<Deformation> deformations;
	SnowflakeModel::Solid solid_out;
	SnowflakeModel::RandomGenerator randgen;
	double D_max;
	double d_max;
	double fill_ratio;
	float E_0;
	float decay_distance;
	double fill;
	size_t rejected;
	size_t pass;
	std::unique_ptr<SnowflakeModel::FileOut> file_out;

	std::string statfile;
	std::string objfile;
	std::string icefile;
	};

Simstate::Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line
	,SnowflakeModel::Solid&& prototype):r_cmd_line(cmd_line),m_prototype(std::move(prototype))
	,rejected(0),pass(0)
	{
		{
		const auto& x=cmd_line.get<Alice::Stringkey("deformations")>();
		if(!x)
			{throw "No deformation is given. Use --params-show to list the name of availible parameters";}
		deformations=x.valueGet();
		}

	r_cmd_line.print();

	D_max=cmd_line.get<Alice::Stringkey("D_max")>().valueGet();

	fill_ratio=cmd_line.get<Alice::Stringkey("fill-ratio")>().valueGet();
	E_0=cmd_line.get<Alice::Stringkey("E_0")>().valueGet();
	decay_distance=
		std::max(cmd_line.get<Alice::Stringkey("decay_distance")>().valueGet()
			,1.0e-7);
	
		

	fill=0;
	
		{
		randgen.seed(cmd_line.get<Alice::Stringkey("seed")>().valueGet());
		randgen.discard(65536);
		}

		{
		auto p=particleGenerate(m_prototype,deformations,randgen);
		solid_out.merge(p.solidGet(),0,0);
		}
		{
		auto ex=solid_out.extremaGet();
		d_max=length(ex.first - ex.second);
		}

	statfile=r_cmd_line.get<Alice::Stringkey("dump-stats")>().valueGet();
	if(statfile.size())
		{
		file_out=std::make_unique<SnowflakeModel::FileOut>(statfile.c_str());
		file_out->printf("# R_max\tD_max\tVolume\tL_x\tr_xy\tr_xz\tNumber of sub-volumes\tOverlap count\n");
		}
	objfile=r_cmd_line.get<Alice::Stringkey("dump-geometry")>().valueGet();
	icefile=r_cmd_line.get<Alice::Stringkey("dump-geometry-ice")>().valueGet();
	}

Simstate::Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line
	,const std::string& statefile):r_cmd_line(cmd_line)
	{
	SnowflakeModel::DataDump dump(statefile.c_str(),SnowflakeModel::DataDump::IOMode::READ);
	m_prototype=SnowflakeModel::Solid(dump,"prototype");
	solid_out=SnowflakeModel::Solid(dump,"solid_out");
	dump.arrayRead<uint32_t>("randgen_state")
		.dataRead(SnowflakeModel::get(randgen).state,SnowflakeModel::get(randgen).size());
	dump.arrayRead<size_t>("randgen_position")
		.dataRead(&SnowflakeModel::get(randgen).position,1);
	dump.arrayRead<double>("D_max").dataRead(&D_max,1);
	dump.arrayRead<double>("fill_ratio").dataRead(&fill_ratio,1);
	dump.arrayRead<size_t>("rejected").dataRead(&rejected,1);
	dump.arrayRead<size_t>("pass").dataRead(&pass,1);
	dump.arrayRead<float>("E_0").dataRead(&E_0,1);
	dump.arrayRead<float>("decay_distance").dataRead(&decay_distance,1);

	statfile=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("statfile")[0];
	objfile=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("objfile")[0];
	icefile=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("icefile")[0];

	if(statfile.size())
		{
		file_out=std::make_unique<SnowflakeModel::FileOut>(statfile.c_str()
			,SnowflakeModel::FileOut::OpenMode::APPEND);
		}

		{
		auto defs_in=dump.arrayGet<DeformationDataIn>("deformations");
		deformations.resize(defs_in.size());
		auto ptr_in=defs_in.data();
		auto ptr_end=ptr_in + defs_in.size();
		auto defs_out=deformations.data();
		while(ptr_in!=ptr_end)
			{
			*defs_out={std::string(ptr_in->name),ptr_in->mean,ptr_in->standard_deviation};
			++defs_out;
			++ptr_in;
			}
		}
	}


void Simstate::save(const std::string& now) const
	{
	auto statefile=r_cmd_line.get<Alice::Stringkey("statefile")>().valueGet();
	auto filename_dump=statefileName(statefile,now);
	fprintf(stderr,"# Dumping simulation state to %s\n",filename_dump.c_str());
	SnowflakeModel::DataDump dump(filename_dump.c_str()
		,SnowflakeModel::DataDump::IOMode::WRITE);
	m_prototype.write("prototype",dump);
	solid_out.write("solid_out",dump);
	auto& rng_state=SnowflakeModel::get(randgen);
	dump.write("randgen_state",rng_state.state,rng_state.size());
	dump.write("randgen_position",&rng_state.position,1);
	dump.write("D_max",&D_max,1);
	dump.write("fill_ratio",&fill_ratio,1);
	dump.write("rejected",&rejected,1);
	dump.write("pass",&pass,1);
	dump.write("E_0",&E_0,1);
	dump.write("decay_distance",&decay_distance,1);

		{
		auto x=statfile.c_str();
		dump.write("statfile",&x,1);
		}

		{
		auto x=objfile.c_str();
		dump.write("objfile",&x,1);
		}

		{
		auto x=icefile.c_str();
		dump.write("icefile",&x,1);
		}

		{
		auto x=r_cmd_line.get<Alice::Stringkey("prototype")>().valueGet().c_str();
		dump.write("prototype_source",&x,1);
		}

		{
		auto N=deformations.size();
		std::vector<DeformationDataOut> defs_out(N);
		auto ptr_out=defs_out.data();
		auto ptr=deformations.data();
		auto ptr_end=ptr+N;
		while(ptr!=ptr_end)
			{
			*ptr_out={ptr->name.c_str(),ptr->mean,ptr->standard_deviation};
			++ptr_out;
			++ptr;
			}
		dump.write("deformations",defs_out.data(),N);
		}
	}

void Simstate::step()
	{
	auto p=particleGenerate(m_prototype,deformations,randgen);
	std::uniform_real_distribution<float> U(0,E_0);
	auto E=U(randgen);
	auto T_a=faceChoose(solid_out,randgen,E,decay_distance,1);
	if(T_a.second==INFINITY)
		{
		++pass;
		return;
		}
	auto T_b=faceChoose(p.solidGet(),randgen,0.0f,1.0f,0);
	if(T_b.second==INFINITY)
		{return;}


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
		if(d_max<D_max)
			{
			solid_out.merge(std::move(obj),0,0);
			auto ex=solid_out.extremaGet();
			d_max=length(ex.first - ex.second);
			fill=solid_out.volumeGet()/( 4*std::acos(-1.0)*pow(0.5*d_max,3)/3.0 );
			fprintf(stderr,"\r%.7g %.7g  (Cs: %zu,  Cr: %zu, Pass: %zu)      "
				,d_max,fill,solid_out.subvolumesCount(),rejected,pass);
			fflush(stderr);
			statsDump(file_out.get(),solid_out);
			}
		else
			{
			auto s_a_temp=solid_out;
			s_a_temp.merge(std::move(obj),0,0);
			auto ex=s_a_temp.extremaGet();
			if(length(ex.first - ex.second)>d_max)
				{return;}
			solid_out=std::move(s_a_temp);
			fill=solid_out.volumeGet()/( 4*std::acos(-1.0)*pow(0.5*d_max,3)/3.0 );
			fprintf(stderr,"\r%.7g %.7g  (Cs: %zu,  Cr: %zu)      "
				,d_max,fill,solid_out.subvolumesCount(),rejected);
			fflush(stderr);
			statsDump(file_out.get(),solid_out);
			}
		}
	else
		{++rejected;}
	}

void Simstate::objfileWrite() const
	{
	if(objfile.size())
		{
		fprintf(stderr,"# Dumping wavefront file\n");
		SnowflakeModel::FileOut dest(objfile.c_str());
		SnowflakeModel::SolidWriter writer(dest);
		writer.write(solid_out);
		}
	}

void Simstate::icefileWrite() const
	{
	if(icefile.size())
		{
		fprintf(stderr,"# Dumping crystal prototype file\n");
		SnowflakeModel::FileOut file_out(icefile.c_str());
		SnowflakeModel::SolidWriterPrototype writer(file_out);
		writer.write(solid_out);
		}
	}

static std::pair<Simstate,bool> simstateCreate(const Alice::CommandLine<OptionDescriptor>& cmd_line)
	{
	auto& statefile=cmd_line.get<Alice::Stringkey("statefile")>();
	if(statefile)
		{
		return std::pair<Simstate,bool>{std::move( Simstate(cmd_line,statefile.valueGet() ) ),1};
		}
	auto prototype=prototypeLoad(cmd_line);
	if(paramsShow(cmd_line,prototype))
		{return std::pair<Simstate,bool>{Simstate(cmd_line),0};}

	return std::pair<Simstate,bool>{Simstate(cmd_line,std::move( prototype ) ),1};
	}


int main(int argc,char** argv)
	{
	try
		{
		Alice::CommandLine<OptionDescriptor> cmd_line(argc,argv);
		if(printHelp(cmd_line))
			{return 0;}

		auto state=simstateCreate(cmd_line);
		if(state.second==0)
			{return 0;}

		auto now=SnowflakeModel::getdate();
		fprintf(stderr,"# Simulation started %s\n",now.c_str());
		SnowflakeModel::CtrlCHandler int_handler;
		while(state.first.progressGet()<1.0 && !int_handler.captured())
			{
			state.first.step();
			}
		
		state.first.save(now);
		state.first.objfileWrite();
		state.first.icefileWrite();
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
