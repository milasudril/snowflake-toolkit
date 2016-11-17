//@	{
//@	"targets":
//@		[{
//@		 "name":"graupel_generate2"
//@		,"type":"application"
//@		,"description":"Graupel generator"
//@		}]
//@	}

#include "file_in.h"
#include "file_out.h"
#include "sphere_aggregate_writer.h"
#include "sphere_aggregate.h"
#include "randomgenerator.h"
#include "solid_writer_prototype.h"
#include "ctrlchandler.h"
#include "filenameesc.h"
#include "getdate.h"
#include "solid_writer.h"
#include "solid.h"
#include "filename.h"
#include "probability_map.h"
#include "pmap_loader.h"
#include "alice/commandline.hpp"

#include <fenv.h>

struct ParameterValue
	{
	float mean;
	float standard_deviation;
	};

enum class Projection:int{RAW,CYLINDRICAL};


namespace Alice
	{
	void print(const ParameterValue& deformation,FILE* dest)
		{
		fprintf(dest,"{\"mean\":%.7g,\"standard_deviation\":%.7g}"
			,deformation.mean
			,deformation.standard_deviation);
		}

	void print(Projection proj,FILE* output)
		{fprintf(output,"\"%s\"",proj==Projection::RAW?"raw":"cylindrical");};

	template<>
	struct MakeType<Stringkey("Parameter value")>:public MakeTypeBase
		{
		typedef ParameterValue Type;
		static constexpr const char* descriptionShortGet() noexcept
			{return "mean,standard deviation";}
		};

	template<class ErrorHandler>
	struct MakeValue<ParameterValue,ErrorHandler>
		{
		static ParameterValue make_value(const std::string& str);
		};

	template<class ErrorHandler>
	ParameterValue MakeValue<ParameterValue,ErrorHandler>::make_value(const std::string& str)
		{
		ParameterValue ret{};
		auto ptr=str.data();
		size_t fieldcount=1;
		std::string buffer;
		while(1)
			{
			auto x=*ptr;
			switch(fieldcount)
				{
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
							throw "Too many arguments for parameter";
						case '\0':
							ret.standard_deviation=atof(buffer.c_str());
							return ret;
						default:
							buffer+=x;
						}
					break;
				default:
					throw "Too many arguments for parameter";
				}
			++ptr;
			}

		return ParameterValue{};
		}

	template<>
	struct MakeType<Stringkey("number")>:public MakeType<Stringkey("unsigned int")>
		{};

	template<>
	struct MakeType<Stringkey("projection")>
		{
		typedef Projection Type;

		static constexpr const char* descriptionShortGet()
			{return "raw | spherical";}

		static constexpr const char* descriptionLongGet()
			{
			return "A projection is used for mapping angles to pixels. There are "
				"two availible projections: `raw` and `cylindrical`. The `raw` "
				"projection uses uniform scaling in latitude, while `cylindrical` "
				"preserves the area around the poles.";
			}
		};

	template<class ErrorHandler>
	struct MakeValue<Projection,ErrorHandler>
		{
		static Projection make_value(const std::string& str)
			{
			if(str=="raw")
				{return Projection::RAW;}
			if(str=="cylindrical")
				{return Projection::CYLINDRICAL;}
			throw "Invalid projection";
			}
		};
	}

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	
	,{"Simulation parameters","seed","Random seed mod 2^32","unsigned int",Alice::Option::Multiplicity::ONE}

	,{"Simulation parameters","pmap","Gives a linear grayscale PNG file to use for direction probabilities. "
		"Without this option, a uniform distribution is used."
		,"filename",Alice::Option::Multiplicity::ONE}

	,{"Simulation parameters","projection","Sets a projection for the probability map. "
		"If not set, a cylindrical projection is used."
		,"projection",Alice::Option::Multiplicity::ONE}
	
	,{"Simulation parameters","scale","Determines the diameter of individual spheres","Parameter value",Alice::Option::Multiplicity::ONE}
	
	,{"Simulation parameters","E_0","The maximal initial particle energe. The energy is chosen as U(0, E_0)","float",Alice::Option::Multiplicity::ONE}
	
	,{"Simulation parameters","decay-distance","The number of units before the energy has decayed to 1/e","float",Alice::Option::Multiplicity::ONE}
	
	,{"Simulation parameters","merge-offset","The merge offset mearured in, and relative to the radius of "
		"the particle that is being added to the aggregate. A value of zero corresponds to a perfect alignment. "
		"A positive value will result in an overlap, while a negative value will result in a gap. Notice that a "
		"value less than or equal to zero may still result in one or more overlaps."
		,"float",Alice::Option::Multiplicity::ONE}

	,{"Simulation parameters","overlap-max","The maximum number of overlaps that is accepted. If merge-offset "
		"is greater than zero, the maximum numer of overlaps has to be non-zero.","number",Alice::Option::Multiplicity::ONE}
	
	,{"Simulation parameters","D_max","Generate a graupel of diameter D_max. D_max is defined as the largest distance between two vertices.","float",Alice::Option::Multiplicity::ONE}
	
	,{"Simulation parameters","fill-ratio","Set minimum fill ratio of the bounding sphere. This option "
		"is used when D_max has been reached, to increase the total mass. "
		"If the fill ratio is non-zero, and D_max is fullfilled, only events that do not increase D_max are accepted.","double"
		,Alice::Option::Multiplicity::ONE}

	,{"Output options","dump-geometry","Specify output Wavefront file","filename",Alice::Option::Multiplicity::ONE}
	
	,{"Output options","dump-geometry-ice","Same as --dump-geometry but write data as a sphere aggregate file, "
		"so it can processed by sphere_aggregate_rasterize.","filename",Alice::Option::Multiplicity::ONE}
	
	,{"Output options","dump-stats","Write statistics to the given file","filename",Alice::Option::Multiplicity::ONE}

	,{"Output options","report-rate","Limit statistics to be written every `number` step","number",Alice::Option::Multiplicity::ONE}
	
	,{"Other","statefile-in","Reload state from file","filename",Alice::Option::Multiplicity::ONE}

	,{"Other","statefile-out","Save state to file when exiting","filename",Alice::Option::Multiplicity::ONE}
	);




namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<ParameterValue>::fields[]=
		{
		 {"mean",offsetOf(&ParameterValue::mean),DataDump::MetaObject<decltype(ParameterValue::mean)>().typeGet()}
		,{"standard_deviation",offsetOf(&ParameterValue::standard_deviation),DataDump::MetaObject<decltype(ParameterValue::standard_deviation)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<ParameterValue>::field_count=2;
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

static SnowflakeModel::Vector mapProjection(float xi,float eta
	,Projection proj)
	{
	if(proj==Projection::CYLINDRICAL)
		{
		auto xi_prime=xi*2.0f*std::acos(-1.0f);
		auto eta_prime=2.0f*eta - 1.0f;
		auto r=std::sqrt(1.0f - eta_prime*eta_prime);
		return SnowflakeModel::Vector
			{
			 std::cos(xi_prime)*r
			,std::sin(xi_prime)*r
			,eta_prime
			};
		}
	else
		{
		auto xi_prime=xi*2.0f*std::acos(-1.0f);
		auto eta_prime=std::acos(-1.0f)*eta;
		return 
			{
			 std::cos(xi_prime)*std::sin(eta_prime)
			,std::sin(xi_prime)*std::sin(eta_prime)
			,std::cos(eta_prime)
			};
		}
	}

static SnowflakeModel::Vector drawSphere(SnowflakeModel::RandomGenerator& randgen
	,Projection proj)
	{
	std::uniform_real_distribution<float> xi_dist(0,2.0f*std::acos(-1.0f));
	std::uniform_real_distribution<float> eta_dist(-1.0f,1.0f);
	
	return mapProjection(xi_dist(randgen),eta_dist(randgen),proj);
	}

static SnowflakeModel::Vector drawSphere(SnowflakeModel::RandomGenerator& randgen
	,const SnowflakeModel::ProbabilityMap<float>& pmap
	,Projection proj)
	{
	auto n_rows=pmap.nRowsGet();
	auto n_cols=pmap.nColsGet();

	if(n_rows==1 || n_cols==1)
		{return drawSphere(randgen,proj);}

	auto elem=SnowflakeModel::elementChoose(randgen,pmap);
	auto xi=(elem.second + 0.5f)/n_cols;
	auto eta=(elem.first + 0.5f)/n_rows;
	return mapProjection(xi,eta,proj);
	}

template<class T,class RandomGenerator>
static auto shootRandom(const T& object,RandomGenerator& randgen
	,const SnowflakeModel::ProbabilityMap<float>& pmap,Projection proj
	,float E_0,float decay_distance)
	{
//	Construct a sphere from the bounding box
	auto& bb=object.boundingBoxGet();
	auto bb_size=bb.sizeGet();
	auto bb_mid=bb.centerGet();
	auto r=0.5f*glm::length( bb_size );

	auto direction=drawSphere(randgen,pmap,proj);


//	Set the source on the sphere
	auto source=bb_mid - SnowflakeModel::Point(r*direction,0.0f);

	return object.shoot(source,direction,E_0,decay_distance);
	}

static void statsDump(SnowflakeModel::FileOut* file_out,const SnowflakeModel::SphereAggregate& solid)
	{
	if(file_out!=nullptr)
		{
		auto& bb=solid.boundingBoxGet();
		auto extrema=solid.extremaGet();
		auto L=bb.m_max-bb.m_min;
		fflush(stdout);
		file_out->printf(
			"%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%.7g\t"
			"%zu\n"
			,glm::distance(extrema.first,extrema.second)
			,solid.volumeGet()
			,L.x,L.x/L.y,L.x/L.z,solid.subvolumesCount());
		}
	}

static std::gamma_distribution<float> generateGamma(float E,float sigma)
	{
//	Parameter setup from Wikipedia article.
//	Use the standard deviation from user input:
//		sigma^2=k*theta^2
//	we have
	float k=E*E/(sigma*sigma);
	float theta=sigma*sigma/E;
//	Convert to C++ notation
	auto beta=theta;
	float alpha=k;
	return std::gamma_distribution<float>(alpha,beta);
	}

template<class Distribution,class RandomGenerator>
static SnowflakeModel::Sphere particleGenerate(Distribution& d,RandomGenerator& rng)
	{
	return SnowflakeModel::Sphere(SnowflakeModel::Point(0.0f,0.0f,0.0f,1.0f),d(rng));
	}


struct Simstate
	{
	Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line);

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
	
	SnowflakeModel::RandomGenerator randgen;
	SnowflakeModel::ProbabilityMap<float> pmap;
	Projection proj;
	ParameterValue scale;
	std::gamma_distribution<float> G;
	SnowflakeModel::SphereAggregate solid_out;
	float D_max;
	float d_max;
	double fill;
	double fill_ratio;
	float E_0;
	float decay_distance;
	float merge_offset;
	unsigned int overlap_max;
	
	size_t rejected;
	size_t pass;
	std::unique_ptr<SnowflakeModel::FileOut> file_out;

	std::string statfile;
	std::string objfile;
	std::string icefile;
	size_t iter_count;
	size_t report_rate;
	};

Simstate::Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line):
	r_cmd_line(cmd_line),rejected(0),pass(0),iter_count(0)
	{
		{
		const auto& x=cmd_line.get<Alice::Stringkey("scale")>();
		if(!x)
			{throw "No scale parameter is given";}
		scale=x.valueGet();
		scale.mean/=2.0f;
		scale.standard_deviation/=2.0f;
		}

	G=generateGamma(scale.mean,scale.standard_deviation);

	r_cmd_line.print();

	D_max=cmd_line.get<Alice::Stringkey("D_max")>().valueGet();

	fill_ratio=cmd_line.get<Alice::Stringkey("fill-ratio")>().valueGet();
	E_0=cmd_line.get<Alice::Stringkey("E_0")>().valueGet();
	decay_distance=
		std::max(cmd_line.get<Alice::Stringkey("decay-distance")>().valueGet()
			,1.0e-7f);

	fill=0;
	
		{
		auto& x=cmd_line.get<Alice::Stringkey("seed")>();
		if(x)
			{randgen.seed(x.valueGet());}
		randgen.discard(65536);
		}

		{
		merge_offset=1.0f;
		auto& x=cmd_line.get<Alice::Stringkey("merge-offset")>();
		if(x)
			{merge_offset=(-x.valueGet() + 1.0f);}
		}

		{
		auto& x=cmd_line.get<Alice::Stringkey("overlap-max")>();
		if(x)
			{overlap_max=x.valueGet();}

		if(merge_offset<1.0f && overlap_max==0)
			{throw "A positive merge offset requires at least one overlap section";}
		}

		{
		auto p=particleGenerate(G,randgen);
		solid_out.subvolumeAdd(p,0.0f);
		}

		{
		auto ex=solid_out.extremaGet();
		d_max=length(ex.first - ex.second);
		}

	statfile=r_cmd_line.get<Alice::Stringkey("dump-stats")>().valueGet();
	if(statfile.size())
		{
		file_out=std::make_unique<SnowflakeModel::FileOut>(statfile.c_str());
		file_out->printf("D_max\tVolume\tL_x\tr_xy\tr_xz\tNumber_of_subvolumes\n");
		}

		{
		report_rate=128;
		auto& x=r_cmd_line.get<Alice::Stringkey("report-rate")>();
		if(x)
			{
			report_rate=x.valueGet();
			report_rate=std::max(size_t(1),report_rate);
			}
		}

		{
		auto& x=r_cmd_line.get<Alice::Stringkey("pmap")>();
		if(x)
			{
			pmap=SnowflakeModel::pmapLoad(x.valueGet().c_str());
			}
		}

	proj=Projection::CYLINDRICAL;
		{
		auto& x=r_cmd_line.get<Alice::Stringkey("projection")>();
		if(x)
			{proj=x.valueGet();}
		}

	objfile=r_cmd_line.get<Alice::Stringkey("dump-geometry")>().valueGet();
	icefile=r_cmd_line.get<Alice::Stringkey("dump-geometry-ice")>().valueGet();
	}

Simstate::Simstate(const Alice::CommandLine<OptionDescriptor>& cmd_line
	,const std::string& statefile):r_cmd_line(cmd_line)
	{
	SnowflakeModel::DataDump dump(statefile.c_str(),SnowflakeModel::DataDump::IOMode::READ);
	solid_out=SnowflakeModel::SphereAggregate(dump,"solid_out");
	dump.arrayRead<uint32_t>("randgen_state")
		.dataRead(SnowflakeModel::get(randgen).state,SnowflakeModel::get(randgen).size());
	dump.arrayRead<size_t>("randgen_position")
		.dataRead(&SnowflakeModel::get(randgen).position,1);
	dump.arrayRead<float>("D_max").dataRead(&D_max,1);
	dump.arrayRead<double>("fill_ratio").dataRead(&fill_ratio,1);
	dump.arrayRead<size_t>("rejected").dataRead(&rejected,1);
	dump.arrayRead<size_t>("pass").dataRead(&pass,1);
	dump.arrayRead<float>("E_0").dataRead(&E_0,1);
	dump.arrayRead<float>("decay_distance").dataRead(&decay_distance,1);
	dump.arrayRead<float>("merge_offset").dataRead(&merge_offset,1);
	dump.arrayRead<unsigned int>("overlap_max").dataRead(&overlap_max,1);

		{
		uint32_t s[2];
		dump.arrayRead<uint32_t>("pmap_size").dataRead(s,2);
		pmap=SnowflakeModel::ProbabilityMap<float>(s[0],s[1]);
		dump.arrayRead<float>("pmap").dataRead(pmap.rowGet(0),s[0]*s[1]);
		}

	dump.arrayRead<int>("projection").dataRead(reinterpret_cast<int*>(&proj),1);

	statfile=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("statfile")[0];
	objfile=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("objfile")[0];
	icefile=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("icefile")[0];

	if(statfile.size())
		{
		file_out=std::make_unique<SnowflakeModel::FileOut>(statfile.c_str()
			,SnowflakeModel::FileOut::OpenMode::APPEND);
		}

	dump.arrayRead<ParameterValue>("scale").dataRead(&scale,1);
	G=generateGamma(scale.mean,scale.standard_deviation);
	auto ex=solid_out.extremaGet();
	d_max=length(ex.first - ex.second);
	fill=solid_out.volumeGet()/( 4*std::acos(-1.0)*pow(0.5*d_max,3)/3.0 );
	dump.arrayRead<size_t>("iter_count").dataRead(&iter_count,1);
	dump.arrayRead<size_t>("report_rate").dataRead(&report_rate,1);
	report_rate=std::max(size_t(1),report_rate);
	}


void Simstate::save(const std::string& now) const
	{
	auto& statefile=r_cmd_line.get<Alice::Stringkey("statefile-out")>();
	if(!statefile)
		{return;}

	auto& filename_dump=statefile.valueGet();
	fprintf(stderr,"# Dumping simulation state to %s\n",filename_dump.c_str());
	SnowflakeModel::DataDump dump(filename_dump.c_str()
		,SnowflakeModel::DataDump::IOMode::WRITE);
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
	dump.write("merge_offset",&merge_offset,1);
	dump.write("overlap_max",&overlap_max,1);
	dump.write("projection",reinterpret_cast<const int*>(&proj),1);

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
		dump.write("scale",&scale,1);
		}

		{
		dump.write("iter_count",&iter_count,1);
		}

		{
		dump.write("report_rate",&report_rate,1);
		}

		{
		uint32_t s[]={pmap.nRowsGet(),pmap.nColsGet()};
		dump.write("pmap_size",s,2);
		dump.write("pmap",pmap.rowGet(0),s[0]*s[1]);
		}
	}




void Simstate::step()
	{
	std::uniform_real_distribution<float> U(0,E_0);
	auto E=U(randgen);

	auto posnormal=shootRandom(solid_out,randgen,pmap,proj,E,decay_distance);

	if(posnormal.first.x==INFINITY)
		{
		++pass;
		return;
		}

	auto r_2=G(randgen);
	SnowflakeModel::Sphere v_2{posnormal.first + merge_offset*r_2*SnowflakeModel::Point(posnormal.second,0.0f),r_2};

	double overlap_res;
	auto n=overlap(solid_out,v_2,overlap_max,overlap_res);
	if(n<=overlap_max)
		{
		if(d_max<D_max)
			{
			solid_out.subvolumeAdd(std::move(v_2),overlap_res);
			auto ex=solid_out.extremaGet();
			d_max=length(ex.first - ex.second);
			if(iter_count%report_rate==0)
				{
				fill=solid_out.volumeGet()/( 4*std::acos(-1.0)*pow(0.5*d_max,3)/3.0 );
				fprintf(stderr,"\r%.7g %.7g  (Cs: %zu,  Cr: %zu, Pass: %zu)      "
					,d_max,fill,solid_out.subvolumesCount(),rejected,pass);
				fflush(stderr);
				statsDump(file_out.get(),solid_out);
				}
			++iter_count;
			}
		else
			{
			auto ex=solid_out.extremaNew(v_2);
			if(length(ex.first - ex.second)>d_max)
				{return;}
			solid_out.subvolumeAdd(std::move(v_2),overlap_res);
			if(iter_count%report_rate==0)
				{
				fill=solid_out.volumeGet()/( 4*std::acos(-1.0)*pow(0.5*d_max,3)/3.0 );
				fprintf(stderr,"\r%.7g %.7g  (Cs: %zu,  Cr: %zu)      "
					,d_max,fill,solid_out.subvolumesCount(),rejected);
				fflush(stderr);
				statsDump(file_out.get(),solid_out);
				}
			++iter_count;
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
		writer.write(SnowflakeModel::Solid(solid_out,1));
		}
	}

void Simstate::icefileWrite() const
	{
	if(icefile.size())
		{
		fprintf(stderr,"# Dumping spheres\n");
		SnowflakeModel::FileOut file_out(icefile.c_str());
		SnowflakeModel::SphereAggregateWriter writer(file_out);
		writer.write(solid_out);
		}
	}

static Simstate simstateCreate(const Alice::CommandLine<OptionDescriptor>& cmd_line)
	{
	auto& statefile=cmd_line.get<Alice::Stringkey("statefile-in")>();
	if(statefile)
		{
		return Simstate(cmd_line,statefile.valueGet());
		}

	return Simstate(cmd_line);
	}


int main(int argc,char** argv)
	{
	try
		{
#ifndef NDEBUG
		feenableexcept(FE_INVALID | FE_OVERFLOW);
#endif

		Alice::CommandLine<OptionDescriptor> cmd_line(argc,argv);
		if(printHelp(cmd_line))
			{return 0;}

		auto state=simstateCreate(cmd_line);


		auto now=SnowflakeModel::getdate();
		fprintf(stderr,"# Simulation started %s\n",now.c_str());
		SnowflakeModel::CtrlCHandler int_handler;
		while(state.progressGet()<1.0 && !int_handler.captured())
			{
			state.step();
			}
		
		state.save(now);
		state.objfileWrite();
		state.icefileWrite();
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
