//@	{
//@	"targets":
//@		[{
//@		 "name":"snowflake_simulate3","type":"application"
//@		,"description":"The aggregate generator"
//@		}]
//@	}
#include "deformation_data.h"
#include "config_parser.h"
#include "solid_loader.h"
#include "solid_writer.h"
#include "solid_writer_prototype.h"
#include "solid.h"
#include "grid.h"
#include "adda.h"
#include "file_out.h"
#include "ice_particle.h"
#include "twins.h"
#include "matrix_storage_fastsum.h"
#include "element_randomizer.h"
#include "profile.h"
#include "ctrlchandler.h"
#include "getdate.h"
#include "filenameesc.h"
#include "randomgenerator.h"
#include "prototypechoices.h"
#include "alice/commandline.hpp" //Alice will replace getopt later

#include <getopt.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>

#include <map>
#include <cstdlib>
#include <string>
#include <random>
#include <sys/stat.h>

static constexpr char PARAM_HELP='@';
static constexpr char PARAM_SHAPE='A';
static constexpr char PARAM_DEFORMATION='B';
static constexpr char PARAM_OUTPUT_DIR='C';
static constexpr char PARAM_GEOMETRY_SAMPLE='D';
static constexpr char PARAM_GEOMETRY_DUMP='E';
static constexpr char PARAM_STATS_DUMP='F';
static constexpr char PARAM_SEED='G';
static constexpr char PARAM_PARTICLE_COUNT='H';
static constexpr char PARAM_SIGMA='I';
static constexpr char PARAM_DROPRATE='J';
static constexpr char PARAM_GROWTHRATE='K';
static constexpr char PARAM_MELTRATE='L';
static constexpr char PARAM_PARAMSHOW='N';
static constexpr char PARAM_GEOMETRY_DUMP_ICE='O';
static constexpr char PARAM_STATEFILE_IN='P';
static constexpr char PARAM_STATEFILE_OUT='Q';
static constexpr char PARAM_STOPCOND='R';
static constexpr char PARAM_MERGERETRIES='S';
static constexpr char PARAM_OVERLAP_MAX='T';
static constexpr char PARAM_OVERLAP_MIN='U';
static constexpr char PARAM_PROTOTYPECHOICES='V';


static const struct option PROGRAM_OPTIONS[]=
	{
		 {"help",no_argument,nullptr,PARAM_HELP}
		,{"shape",required_argument,nullptr,PARAM_SHAPE}
		,{"deformation",required_argument,nullptr,PARAM_DEFORMATION}
		,{"output-directory",required_argument,nullptr,PARAM_OUTPUT_DIR}

		,{"sample-geometry",required_argument,nullptr,PARAM_GEOMETRY_SAMPLE}
		,{"dump-stats",required_argument,nullptr,PARAM_STATS_DUMP}
		,{"dump-geometry",no_argument,nullptr,PARAM_GEOMETRY_DUMP}
		,{"dump-geometry-ice",no_argument,nullptr,PARAM_GEOMETRY_DUMP_ICE}

		,{"seed",required_argument,nullptr,PARAM_SEED}
		,{"N",required_argument,nullptr,PARAM_PARTICLE_COUNT}
		,{"droprate",required_argument,nullptr,PARAM_DROPRATE}
		,{"growthrate",required_argument,nullptr,PARAM_GROWTHRATE}
		,{"meltrate",required_argument,nullptr,PARAM_MELTRATE}
		,{"param-show",no_argument,nullptr,PARAM_PARAMSHOW}
		,{"statefile-in",required_argument,nullptr,PARAM_STATEFILE_IN}
		,{"statefile-out",required_argument,nullptr,PARAM_STATEFILE_OUT}
		,{"stop-cond",required_argument,nullptr,PARAM_STOPCOND}
		,{"merge-retries",required_argument,nullptr,PARAM_MERGERETRIES}
		,{"overlap-max",required_argument,nullptr,PARAM_OVERLAP_MAX}
		,{"overlap-min",required_argument,nullptr,PARAM_OVERLAP_MIN}
		,{"prototype-choices",required_argument,nullptr,PARAM_PROTOTYPECHOICES}
		,{0,0,0,0}
	};


class Simstate;

class SimstateMonitor
	{
	public:
		virtual double progressGet(const Simstate& state) noexcept=0;
		typedef std::unique_ptr<SimstateMonitor> (*Factory)(const char* argument);
	};

struct Setup
	{
	struct Data
		{
		uint32_t m_stat_saverate;
		uint32_t m_actions;

		unsigned int m_size_x;
		unsigned int m_size_y;
		unsigned int m_size_z;

		size_t m_N;
		uint32_t m_seed;
		float m_droprate;
		float m_growthrate;
		float m_meltrate;
		double m_overlap_min;
		double m_overlap_max;
		size_t m_merge_retries;
		} m_data;


	SnowflakeModel::PrototypeChoices m_prototypes;
	std::string m_output_dir;
	std::string m_statefile_in;
	std::string m_statefile_out;
	std::string m_stopcond_name;
	std::string m_stopcond_arg;
	std::string m_prototype;
	std::string m_prototype_choices;

	static constexpr size_t N=1023;
	static constexpr float DROPRATE=100;
	static constexpr float GROWTHRATE=100;
	static constexpr float MELTRATE=100;

	static constexpr uint32_t HELP_SHOW=1;
	static constexpr uint32_t STATS_DUMP=2;
	static constexpr uint32_t GEOMETRY_DUMP=4;
	static constexpr uint32_t GEOMETRY_SAMPLE=8;
	static constexpr uint32_t PARAM_SHOW=16;
	static constexpr uint32_t GEOMETRY_DUMP_ICE=32;

	Setup(int argc,char** argv);
	Setup(const SnowflakeModel::DataDump& dump);
	Setup()=default;

	void paramsDump();
	void validate() const;
	void write(SnowflakeModel::DataDump& dump);
	};

void Setup::validate() const
	{
	if(m_data.m_growthrate<1e-7)
		{throw "Growthrate must be non-zero";}

	if(m_prototypes.choicesCount()==0 && !(m_data.m_actions&HELP_SHOW))
		{
		throw "Crystal file is not given. "
			"Try --help for more information.";
		}

	if( m_output_dir==""
		&& (m_data.m_actions&GEOMETRY_DUMP || m_data.m_actions&GEOMETRY_SAMPLE
			|| m_data.m_actions&STATS_DUMP))
		{throw "No output directory given";}
	}

Setup::Setup(int argc,char** argv):
	m_data{0}
	{
	int option_index;
	int c;
	const char* sample_geometry=nullptr;
	std::vector<std::string> deformations;
	m_data.m_seed=time(nullptr);

	m_data.m_N=N;
	m_data.m_droprate=DROPRATE;
	m_data.m_growthrate=GROWTHRATE;
	m_data.m_meltrate=MELTRATE;
	m_data.m_stat_saverate=256;
	m_data.m_actions=0;
	m_data.m_overlap_min=0;
	m_data.m_overlap_max=0;
	m_data.m_merge_retries=0;

	while( (c=getopt_long(argc,argv,"",PROGRAM_OPTIONS,&option_index))!=-1)
		{
		switch(c)
			{
			case PARAM_HELP:
				m_data.m_actions|=HELP_SHOW;
				break;

			case PARAM_SHAPE:
				m_prototype=optarg;
				break;

			case PARAM_DEFORMATION:
			//	This option takes sub-options
				deformations.push_back(optarg);
				break;

			case PARAM_OUTPUT_DIR:
				m_output_dir=optarg;
				break;

			case PARAM_GEOMETRY_SAMPLE:
			//	This option takes sub-options.
				sample_geometry=optarg;
				m_data.m_actions|=GEOMETRY_SAMPLE;
				break;

			case PARAM_GEOMETRY_DUMP:
				m_data.m_actions|=GEOMETRY_DUMP;
				break;

			case PARAM_GEOMETRY_DUMP_ICE:
				m_data.m_actions|=GEOMETRY_DUMP_ICE;
				break;

			case PARAM_STATS_DUMP:
				{
				m_data.m_actions|=STATS_DUMP;
				auto freq=atoi(optarg);
				if(freq!=0)
					{m_data.m_stat_saverate=freq;}
				}
				break;

			case PARAM_SEED:
				m_data.m_seed=atoi(optarg);
				break;

			case PARAM_PARTICLE_COUNT:
				m_data.m_N=atoi(optarg);
				break;

			case PARAM_DROPRATE:
				m_data.m_droprate=atof(optarg);
				break;

			case PARAM_GROWTHRATE:
				m_data.m_growthrate=atof(optarg);
				break;

			case PARAM_MELTRATE:
				m_data.m_meltrate=atof(optarg);
				break;

			case PARAM_PARAMSHOW:
				m_data.m_actions|=PARAM_SHOW;
				break;

			case PARAM_STATEFILE_IN:
				m_statefile_in=optarg;
				break;

			case PARAM_STATEFILE_OUT:
				m_statefile_out=optarg;
				break;

			case PARAM_STOPCOND:
				{
				auto temp=std::string(optarg);
				auto pos=temp.find_first_of('=');
				m_stopcond_name=temp.substr(0,pos);
				m_stopcond_arg=temp.substr(pos+1);
				}
				break;

			case PARAM_MERGERETRIES:
				m_data.m_merge_retries=atoll(optarg);
				break;

			case PARAM_OVERLAP_MAX:
				m_data.m_overlap_max=atof(optarg);
				break;

			case PARAM_OVERLAP_MIN:
				m_data.m_overlap_min=atof(optarg);
				break;

			case PARAM_PROTOTYPECHOICES:
				m_prototypes.append(optarg);
				m_prototype_choices=optarg;
				break;

			case '?':
				throw "Invalid parameter given";

			}
		}
	if(m_stopcond_name.size()==0 && m_statefile_in.size()==0
		&& !(m_data.m_actions&HELP_SHOW || m_data.m_actions&PARAM_SHOW) )
		{
		throw "No stop condition is given";
		}

	if(m_data.m_overlap_max<0 || m_data.m_overlap_max > 1)
		{throw "Overlap ratio must be between 0 and 1";}

	if(m_data.m_overlap_min > m_data.m_overlap_max)
		{throw "Minimum overlap ratio has to be less than or equal to the maximum overlap ratio";}

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
							m_data.m_size_x=atoi(temp.data());
							break;
						case 1:
							m_data.m_size_y=atoi(temp.data());
							break;
						case 2:
							m_data.m_size_z=atoi(temp.data());
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
			{m_data.m_size_z=atoi(temp.data());}
		}

	if(m_prototype.size())
		{
		std::vector<SnowflakeModel::DeformationData> defs;
		auto ptr=deformations.data();
		auto ptr_end=ptr+deformations.size();
		while(ptr!=ptr_end)
			{
			defs.push_back(
				Alice::make_value<SnowflakeModel::DeformationData,Alice::CmdLineError>(*ptr));
			++ptr;
			}
		m_prototypes.append(m_prototype.c_str(),1.0,{defs.data(),defs.data() + defs.size()});
		}
	}

void Setup::paramsDump()
	{
	printf("Parameters:\n\n"
		"Prototype:         %s\n"
		"Prototype choices: %s\n"
		"Seed:              %u\n"
		"N:                 %zu\n"
		"droprate:          %.7g\n"
		"growthrate:        %.7g\n"
		"meltrate:          %.7g\n"
		"overlap range:     [%.7g, %.7g]\n"
		"merge retries:     %zu\n"
		,m_prototype.c_str(),m_prototype_choices.c_str()
		,m_data.m_seed,m_data.m_N,m_data.m_droprate,m_data.m_growthrate,m_data.m_meltrate
		,m_data.m_overlap_min,m_data.m_overlap_max,m_data.m_merge_retries);

	if(m_prototype.size() && m_prototype_choices.size()==0)
		{
		printf("\nDeformations:\n");
		auto choice=m_prototypes.choicesBegin();
		auto ptr=choice->deformationsBegin();
		auto ptr_end=choice->deformationsEnd();
		while(ptr!=ptr_end)
			{
			printf("    name=%s, mean=%.7g, standard deviation=%.7g\n"
				,ptr->name.data(),ptr->mean,ptr->standard_deviation);
			++ptr;
			}
		}
	}

void helpShow()
	{
	printf("Options:\n\n"
		"--help\n"
		"    Print this text and exit.\n\n"
		"--statefile-in=file\n"
		"    Continue a started simulation whose state is stored in `file`. The simulation setup "
		"stored in `file` overrides any other command line argument.\n\n"
		"--statefile-out=file\n"
		"    Save current state to `file` when exiting.\n\n"
		"--stop-cond=condition\n"
		"    Defines a stop condition. Possible options are\n"
		"     iterations=iteration_count   Run a fixed number of iterations\n"
		"     subvols_max=N                Run until there is an aggreaget with N elements\n"
		"     d_max_max=d_max              Run until there is an aggreaget with d_max as largest extent\n"
		"     v_max=V                      Run until there is an aggreaget with volume V\n"
		"     n_dropped_max=N              Run until the model has dropped N particles\n"
		"     infinity                     Run forever, or until the simulation is stopped externally\n\n"
		"--shape=crystal_file\n"
		"    Generate data using the shape stored in crystal_file."
		" See the reference manal for information about how to create "
		"such a file.\n\n"
		"--deformation=name,random_distribution\n"
		"    Crystal file parameter to use for size modification. Use\n\n"
		"         --param-show\n\n"
		"to list the name of available parameters. The availible random distributions are\n"
		"    custom,filename  A custom probability distribution stored in a text file. See xytab.txt for an example.\n"
		"    delta,mean  A Dirac delta distribution. This will distribution will always return mean.\n"
		"    exponential,mean  An exponential distribution with a give mean\n"
		"    gamma,mean,std   A gamma distribution with mean and standard deviation\n\n"
		"--prototype-choices=filename\n"
		"    Load prototypes, and deformations from filename. The file has to contain JSON data in the following format\n"
		"[{\"prototype\":\"filename\",\"probability\":probability,\"deformations\":[[deformation],...]},...]\n\n"
		"--output-dir=output_directory\n"
		"    Directory for storing output data\n\n"
		"--dump-stats=N\n"
		"    Write particle measurements every N iteration. Measurements for "
		"the first and last iteration are always saved\n\n"
		"--dump-geometry\n"
		"    Export the output geometry as Wavefront files, that can "
		"be imported into 3D modelling software such as blender(1).\n\n"
		"--dump-geometry-ice\n"
		"    Same as --dump-geometry but write data as Ice crystal prototype "
		"files, so they can be used by other tools provided by the toolkit.\n\n"
		"--sample-geometry=Nx,Ny,Nz\n"
		"    Sample the output geometry to a grid of size Nx x Ny x Nz\n\n"
		"--seed=integer\n"
		"    Sets the random number genererator seed\n\n"
		"--N=integer\n"
		"    The maximum number of crystals\n\n"
		"--droprate=value\n"
		"    Model parameter\n\n"
		"--growthrate=value\n"
		"    Model parameter\n\n"
		"--meltrate=value\n"
		"    Model parameter\n\n"
		"--merge-retries=N\n"
		"    If the first try to merge two aggregates failed to satisfy the overlap "
		"constraint, try again N times without skipping the current event.\n\n"
		"--overlap-min=ratio\n"
		"    When merging particles, only accept events where the ratio of overlapping subvolumes is greater than ratio.\n\n"
		"--overlap-max=ratio\n"
		"    When merging particles, only accept events where the ratio of overlapping subvolumes is less than ratio.\n\n"
		);
	}

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
	const DataDump::FieldDescriptor DataDump::MetaObject<Setup::Data>::fields[]=
		{
		 {"actions",offsetOf(&Setup::Data::m_actions),DataDump::MetaObject<decltype(Setup::Data::m_actions)>().typeGet()}
		,{"stat_saverate",offsetOf(&Setup::Data::m_stat_saverate),DataDump::MetaObject<decltype(Setup::Data::m_stat_saverate)>().typeGet()}
		,{"size_x",offsetOf(&Setup::Data::m_size_x),DataDump::MetaObject<decltype(Setup::Data::m_size_x)>().typeGet()}
		,{"size_y",offsetOf(&Setup::Data::m_size_y),DataDump::MetaObject<decltype(Setup::Data::m_size_y)>().typeGet()}
		,{"size_z",offsetOf(&Setup::Data::m_size_z),DataDump::MetaObject<decltype(Setup::Data::m_size_z)>().typeGet()}
		,{"N",offsetOf(&Setup::Data::m_N),DataDump::MetaObject<decltype(Setup::Data::m_N)>().typeGet()}
		,{"seed",offsetOf(&Setup::Data::m_seed),DataDump::MetaObject<decltype(Setup::Data::m_seed)>().typeGet()}
		,{"droprate",offsetOf(&Setup::Data::m_droprate),DataDump::MetaObject<decltype(Setup::Data::m_droprate)>().typeGet()}
		,{"growthrate",offsetOf(&Setup::Data::m_growthrate),DataDump::MetaObject<decltype(Setup::Data::m_growthrate)>().typeGet()}
		,{"meltrate",offsetOf(&Setup::Data::m_meltrate),DataDump::MetaObject<decltype(Setup::Data::m_meltrate)>().typeGet()}
		,{"overlap_min",offsetOf(&Setup::Data::m_overlap_min),DataDump::MetaObject<decltype(Setup::Data::m_overlap_min)>().typeGet()}
		,{"overlap_max",offsetOf(&Setup::Data::m_overlap_max),DataDump::MetaObject<decltype(Setup::Data::m_overlap_max)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Setup::Data>::field_count=10;
	}

void Setup::write(SnowflakeModel::DataDump& dump)
	{
	auto group=dump.groupCreate("setup");
	dump.write("setup/output_dir",&m_output_dir,1);
	dump.write("setup/prototype",&m_prototype,1);
	dump.write("setup/prototype_choices",&m_prototype_choices,1);
	dump.write("setup/stopcond_name",&m_stopcond_name,1);
	dump.write("setup/stopcond_arg",&m_stopcond_arg,1);
	m_prototypes.write("setup/prototypes",dump);
	dump.write("setup/data",&m_data,1);
	}

Setup::Setup(const SnowflakeModel::DataDump& dump)
	{
	m_data=dump.arrayGet<Setup::Data>("setup/data").at(0);
	m_output_dir=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("setup/output_dir").at(0);
	m_prototype=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("setup/prototype").at(0);
	m_prototype_choices=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("setup/prototype_choices").at(0);
	m_stopcond_name=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("setup/stopcond_name").at(0);
	m_stopcond_arg=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("setup/stopcond_arg").at(0);
	m_prototypes=SnowflakeModel::PrototypeChoices(dump,"setup/prototypes");
	}



size_t U(size_t a,size_t b,std::mt19937& randgen)
	{
	return std::uniform_int_distribution<size_t>(a,b)(randgen);
	}

float U(float a,float b,std::mt19937& randgen)
	{
	return std::uniform_real_distribution<float>(a,b)(randgen);
	}

double U(double a,double b,SnowflakeModel::RandomGenerator& randgen)
	{
	return std::uniform_real_distribution<double>(a,b)(randgen);
	}

SnowflakeModel::Vector randomDirection(SnowflakeModel::RandomGenerator& randgen)
	{
	auto theta=U(0,2*glm::pi<float>(),randgen);
	auto phi=U(0,glm::pi<float>(),randgen);

	return {cos(theta)*sin(phi),sin(theta)*sin(phi),cos(phi)};
	}

float vTermCompute(const SnowflakeModel::Solid& sol,const Setup& setup)
	{
	auto R=sol.rMaxGet();
	auto A=R*R;
	auto V=sol.volumeGet();
	return sqrt(V/A);
	}

SnowflakeModel::IceParticle ice_particlePrepare(Setup& setup,SnowflakeModel::RandomGenerator& randgen)
	{
	SnowflakeModel::IceParticle ice_particle;
	auto choice=setup.m_prototypes.choose(randgen);


	ice_particle.solidSet(choice.solidGet());

//	Setup parameters
		{
		auto deformation=choice.deformationsBegin();
		auto deformation_end=choice.deformationsEnd();
		while(deformation!=deformation_end)
			{
			ice_particle.parameterSet(deformation->name.data()
				,deformation->drawMethod(*deformation,randgen));
			++deformation;
			}
		}

	auto vt=vTermCompute(ice_particle.solidGet(),setup);
	ice_particle.velocitySet(vt*randomDirection(randgen));

	return std::move(ice_particle);
	}


float C_coalesce(size_t k,size_t l
	,const std::vector<SnowflakeModel::IceParticle>& ice_particles)
	{
	auto& g_k=ice_particles[k];
	auto& g_l=ice_particles[l];

	if(g_k.dead() || g_l.dead())
		{return 0;}

	auto& S_k=g_k.solidGet();
	auto R_max_k=S_k.rMaxGet();
	auto v_k=g_k.velocityGet();


	auto& S_l=g_l.solidGet();
	auto R_max_l=S_l.rMaxGet();
	auto v_l=g_l.velocityGet();

	return (R_max_k + R_max_l)*(R_max_k + R_max_l)*glm::distance(v_k,v_l);
	}

float C_drop(size_t k,const std::vector<SnowflakeModel::IceParticle>& ice_particles
	,const Setup::Data& setup,size_t N_particles)
	{
	auto& g_k=ice_particles[k];
	if(g_k.dead())
		{return 0;}

	if(setup.m_N==0)
		{abort();}

	auto v_k=g_k.velocityGet();
	return 2.0f*glm::length(v_k)*setup.m_droprate*pow(N_particles,-1.0f/3);
	}

float C_melt(size_t k,const std::vector<SnowflakeModel::IceParticle>& ice_particles
	,const Setup::Data& setup)
	{
	auto& g_k=ice_particles[k];
	if(g_k.dead())
		{return 0;}

	return 2.0f*setup.m_meltrate/g_k.solidGet().volumeGet();
	}

void matrixRowUpdate(size_t k
	,const std::vector<SnowflakeModel::IceParticle>& ice_particles
	,SnowflakeModel::MatrixStorageFastsum& C_mat
	,const Setup::Data& setup
	,size_t N_particles)
	{
	for(size_t l=0;l<ice_particles.size();++l)
		{
		if(l!=k)
			{C_mat.symmetricAssign(k,l,C_coalesce(k,l,ice_particles));}
		}

	auto l=ice_particles.size();

	C_mat(k,l)=C_melt(k,ice_particles,setup);
	auto elem_current=C_mat.rowGet(l);
	k=0;
	while(k!=l)
		{
		*elem_current=C_drop(k,ice_particles,setup,N_particles);
		++elem_current;
		++k;
		}
	C_mat(l,l)=0;
	}

void matrixDump(const SnowflakeModel::MatrixStorageFastsum& C_mat)
	{
	auto elem_current=C_mat.rowGet(0);
	auto n=C_mat.nColsGet();
	putchar('\n');
	while(elem_current!=C_mat.rowsEnd())
		{
		size_t l=0;
		while(l!=n)
			{
			printf("%.7e ",*elem_current);
			++l;
			++elem_current;
			}
		putchar('\n');
		}
	putchar('\n');
	}

SnowflakeModel::Triangle
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

template<class T>
size_t randomDraw(const T* ptr_begin,const T* ptr_end
	,T p_mass,SnowflakeModel::RandomGenerator& randgen)
	{
	do
		{
		auto r=U(static_cast<T>(0),static_cast<T>(p_mass),randgen);
		auto ptr=ptr_begin;
		auto sum=static_cast<T>(0);
		while(ptr!=ptr_end)
			{
			if(r>=sum && r<*ptr + sum)
				{return ptr-ptr_begin;}
			sum+=*ptr;
			++ptr;
			}
		fprintf(stderr,"# WARNING: no event matched. Trying again.\n");
		}
	while(1);
	}

static SnowflakeModel::Twins<size_t>
ice_particlesChoose(const SnowflakeModel::MatrixStorageFastsum& M
	,SnowflakeModel::RandomGenerator& randgen)
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();
	return elementChoose(randgen,M);
	}

glm::vec2 drawFromTriangle(SnowflakeModel::RandomGenerator& randgen)
	{
	float xi=0;
	float eta=0;
	do
		{
		xi=U(0.0f,1.0f,randgen);
		eta=U(0.0f,1.0f,randgen);
		}
	while(eta > 1-xi);

	return glm::vec2{xi,eta};
	}


class Simstate
	{
	public:
		struct Data
			{
			double tau;
			size_t frame;
			size_t N_particles;
			size_t N_particles_dropped;
			size_t collisions;
			size_t collisions_rejected;
			size_t births;
			size_t melts;
			size_t dropouts;
			};

		template<class T>
		Simstate(Setup&& setup,T)=delete;

		Simstate(Setup& setup,const SnowflakeModel::DataDump& dump
			,std::unique_ptr<SimstateMonitor>&& monitor);
		Simstate(Setup& setup,std::unique_ptr<SimstateMonitor>&& monitor);

		bool step();
		void statsDump(bool force=0) const;
		void prototypesDump() const;

		double progressGet() const noexcept
			{return m_monitor->progressGet(*this);}

		void geometryDump() const;

		void rasterize() const;

		void write(SnowflakeModel::DataDump& dump) const;

		size_t frameCurrentGet() const noexcept
			{return m_data.frame;}

		const SnowflakeModel::IceParticle* particlesBegin() const noexcept
			{return ice_particles.data();}

		const SnowflakeModel::IceParticle* particlesEnd() const noexcept
			{return ice_particles.data() + ice_particles.size();}

		size_t nDroppedGet() const noexcept
			{return m_data.N_particles_dropped;}

	private:
		Setup& r_setup;
		std::unique_ptr<SimstateMonitor> m_monitor;
		Data m_data;
		SnowflakeModel::MatrixStorageFastsum C_mat;
		SnowflakeModel::RandomGenerator randgen;
		std::vector<SnowflakeModel::IceParticle> ice_particles;
	//	SnowflakeModel::IdGenerator<unsigned int> m_id_gen;
		std::uniform_int_distribution<int> U_rot; //Stateless
		std::unique_ptr<SnowflakeModel::FileOut> frame_data_file; //Stateless.
		std::unique_ptr<SnowflakeModel::FileOut> dropped_stats;

		friend class SnowflakeModel::DataDump::MetaObject<Data>;
	};

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<Simstate::Data>::fields[]=
		{
		 {"tau",offsetOf(&Simstate::Data::tau),DataDump::MetaObject<decltype(Simstate::Data::tau)>().typeGet()}
		,{"frame",offsetOf(&Simstate::Data::frame),DataDump::MetaObject<decltype(Simstate::Data::frame)>().typeGet()}
		,{"N_particles",offsetOf(&Simstate::Data::N_particles),DataDump::MetaObject<decltype(Simstate::Data::N_particles)>().typeGet()}
		,{"N_particles_dropped",offsetOf(&Simstate::Data::N_particles_dropped),DataDump::MetaObject<decltype(Simstate::Data::N_particles_dropped)>().typeGet()}
		,{"collisions",offsetOf(&Simstate::Data::collisions),DataDump::MetaObject<decltype(Simstate::Data::collisions)>().typeGet()}
		,{"collisions_rejected",offsetOf(&Simstate::Data::collisions_rejected),DataDump::MetaObject<decltype(Simstate::Data::collisions_rejected)>().typeGet()}
		,{"births",offsetOf(&Simstate::Data::births),DataDump::MetaObject<decltype(Simstate::Data::births)>().typeGet()}
		,{"melts",offsetOf(&Simstate::Data::melts),DataDump::MetaObject<decltype(Simstate::Data::melts)>().typeGet()}
		,{"dropouts",offsetOf(&Simstate::Data::dropouts),DataDump::MetaObject<decltype(Simstate::Data::dropouts)>().typeGet()}		
		};

	template<>
	const size_t DataDump::MetaObject<Simstate::Data>::field_count=9;
	}

Simstate::Simstate(Setup& setup
	,const SnowflakeModel::DataDump& dump
	,std::unique_ptr<SimstateMonitor>&& monitor):
	r_setup(setup),m_monitor(std::move(monitor)),C_mat(setup.m_data.m_N+1,setup.m_data.m_N+1)
	,U_rot(0,5)
	{
	dump.arrayRead<Data>("simstate/data")
		.dataRead(&m_data,1);
	dump.matrixGet<double>("simstate/C_mat",C_mat.rowGet(0),C_mat.nRowsGet()
		,C_mat.nColsGet());
	dump.arrayRead<uint32_t>("simstate/randgen_state")
		.dataRead(SnowflakeModel::get(randgen).state,SnowflakeModel::get(randgen).size());
	dump.arrayRead<size_t>("simstate/randgen_position")
		.dataRead(&SnowflakeModel::get(randgen).position,1);

		{
		auto group=dump.groupOpen("simstate/ice_particles");
		auto N_objs=dump.objectsCount(*group);
		ice_particles.resize(std::max(N_objs,setup.m_data.m_N));
		auto group_name=std::string("simstate/ice_particles/");
		dump.iterate(*group,[this,&group_name,&dump](const char* name)
			{
			auto id=group_name+name;
			size_t k=strtol(name,nullptr,16);
			ice_particles[k]=SnowflakeModel::IceParticle(dump,id.c_str());
			});
		}

	if(setup.m_data.m_actions&Setup::STATS_DUMP)
		{
		frame_data_file.reset(new SnowflakeModel::FileOut(
				 (setup.m_output_dir+"/frame_data.txt").data()
				,SnowflakeModel::FileOut::OpenMode::APPEND
			));
		dropped_stats.reset(new SnowflakeModel::FileOut(
				 (setup.m_output_dir+"/dropped_stats.txt").data()
				,SnowflakeModel::FileOut::OpenMode::APPEND
			));
		}
	}

void Simstate::write(SnowflakeModel::DataDump& dump) const
	{
	auto group=dump.groupCreate("simstate");
	dump.write("simstate/data",&m_data,1);
	dump.write("simstate/C_mat",C_mat.rowGet(0),C_mat.nRowsGet(),C_mat.nColsGet());
	dump.write("simstate/randgen_state"
		,SnowflakeModel::get(randgen).state,SnowflakeModel::get(randgen).size());
	dump.write("simstate/randgen_position"
		,&SnowflakeModel::get(randgen).position,1);

		{
		auto ptr=ice_particles.data();
		auto ptr_end=ptr+ice_particles.size();
		auto group=dump.groupCreate("simstate/ice_particles");
		auto group_name=std::string("simstate/ice_particles/");
		size_t k=0;
		while(ptr!=ptr_end)
			{
			if(!ptr->dead())
				{
				char id[32];
				sprintf(id,"%016zx",k);
				auto name=group_name+id;
				ptr->write(name.c_str(),dump);
				}
			++ptr;
			++k;
			}
		}
	}



Simstate::Simstate(Setup& setup,std::unique_ptr<SimstateMonitor>&& monitor):
	 r_setup(setup),m_monitor(std::move(monitor)),m_data{0}
	,C_mat(setup.m_data.m_N+1,setup.m_data.m_N+1)
	,randgen(setup.m_data.m_seed),U_rot(0,5)
	{
//	http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
	randgen.discard(65536);

	ice_particles.resize(setup.m_data.m_N);

	for(size_t k=0;k<ice_particles.size();++k)
		{C_mat(k,k)=2.0f*setup.m_data.m_growthrate/ice_particles.size();}

	if(setup.m_data.m_actions&Setup::STATS_DUMP)
		{
		mkdir(setup.m_output_dir.data(),S_IRWXU|S_IRGRP|S_IXGRP);
		frame_data_file.reset(new SnowflakeModel::FileOut(
				(setup.m_output_dir+"/frame_data.txt").data()
			));
		dropped_stats.reset(new SnowflakeModel::FileOut(
				(setup.m_output_dir+"/dropped_stats.txt").data()
			));

		frame_data_file->printf("Frame\tClock time\tSimulation time\tN_cloud\tN_drop\tCollisions\tCollisions rejected\tBirths\tMelts\tDropouts\n");
		dropped_stats->printf("Frame\tClock time\tSimulation time\tR_max\tVolume\tSpeed\tL_x\tr_xy\tr_xz\tNumber of sub-volumes\n");
		}
	}

void Simstate::geometryDump() const
	{	
		{
		auto i=ice_particles.begin();
		size_t count=0;
		while(i!=ice_particles.end())
			{
			if(!i->dead())
				{
				char num_buff[16];
				sprintf(num_buff,"/mesh-%zx.obj",count);
				SnowflakeModel::FileOut file_out(
					(r_setup.m_output_dir+num_buff).data());
				SnowflakeModel::SolidWriter writer(file_out);
				writer.write(i->solidGet());
				++count;
				}
			++i;
			}
		}
	}

void Simstate::prototypesDump() const
	{
	auto i=ice_particles.begin();
	size_t count=0;
	while(i!=ice_particles.end())
		{
		if(!i->dead())
			{
			char num_buff[16];
			sprintf(num_buff,"/mesh-%zx.ice",count);
			SnowflakeModel::FileOut file_out((r_setup.m_output_dir+num_buff).data());
			SnowflakeModel::SolidWriterPrototype writer(file_out);
			writer.write(i->solidGet());
			++count;
			}
		++i;
		}
	}

void Simstate::rasterize() const
	{
	auto i=ice_particles.begin();
	size_t count=0;
	while(i!=ice_particles.end())
		{
		if(!i->dead())
			{
			SnowflakeModel::Grid grid(
				 r_setup.m_data.m_size_x,r_setup.m_data.m_size_y,r_setup.m_data.m_size_z
				,i->solidGet().boundingBoxGet());

			i->solidGet().geometrySample(grid);
			char num_buff[32];
			sprintf(num_buff,"/geom-%zx.adda",count);
			addaShapeWrite(grid,SnowflakeModel::FileOut((r_setup.m_output_dir+num_buff).data()));
			++count;
			}
		++i;
		}
	}

void Simstate::statsDump(bool force) const
	{
	if(frame_data_file==nullptr 
		|| (m_data.frame%r_setup.m_data.m_stat_saverate!=0 && !force))
		{return;}

	auto now=time(nullptr);

	//	Frame\tClock time\tSimulation time\tN_cloud\tN_drop\t
		frame_data_file->printf(
			"%zu\t"
			"%zu\t"
			"%.15g\t"
			"%zu\t"
			"%zu\t"
			"%zu\t"
			"%zu\t"
			"%zu\t"
			"%zu\t"
			"%zu\n",m_data.frame,now,m_data.tau,m_data.N_particles
			,m_data.N_particles_dropped
			,m_data.collisions
			,m_data.collisions_rejected
			,m_data.births
			,m_data.melts
			,m_data.dropouts);

			{
			char countbuff[32];
			sprintf(countbuff,"/frame-%zu.txt",m_data.frame);
			SnowflakeModel::FileOut file_out((r_setup.m_output_dir+countbuff).data());
			auto i=ice_particles.begin();
			file_out.printf("R_max\tD_max\tVolume\tSpeed\tL_x\tr_xy\tr_xz\tNumber of sub-volumes\tOverlap count\n");
			while(i!=ice_particles.end())
				{
				if(!i->dead())
					{
					auto& solid=i->solidGet();
					auto& bb=solid.boundingBoxGet();
					auto extrema=solid.extremaGet();
					auto L=bb.m_max-bb.m_min;
					file_out.printf("%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%zu\t%zu\n"
						,solid.rMaxGet()
						,glm::distance(extrema.first,extrema.second)
						,solid.volumeGet()
						,glm::length(i->velocityGet())
						,L.x,L.x/L.y,L.x/L.z,solid.subvolumesCount(),solid.overlapCount());
					}
				++i;
				}
			}
	}

static void particleDump(const SnowflakeModel::IceParticle& i,const char* prefix,size_t id)
	{
	std::string name(prefix);
	char id_str[32];
	sprintf(id_str,"%016zx",id);
	name+=id_str;
		{
		SnowflakeModel::FileOut file_out(
			(name+".obj").data());
		SnowflakeModel::SolidWriter writer(file_out);
		writer.write(i.solidGet());
		}

		{
		SnowflakeModel::FileOut file_out(
			(name+".ice").data());
		SnowflakeModel::SolidWriterPrototype writer(file_out);
		writer.write(i.solidGet());
		}
	}

static void particleDumpStats(const SnowflakeModel::IceParticle& i,SnowflakeModel::FileOut& file_out)
	{
	auto& bb=i.solidGet().boundingBoxGet();
	auto L=bb.m_max-bb.m_min;
	file_out.printf("%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%zu\t%zu\n"
		,i.solidGet().rMaxGet()
		,i.solidGet().volumeGet()
		,glm::length(i.velocityGet())
		,L.x,L.x/L.y,L.x/L.z,i.solidGet().subvolumesCount(),i.solidGet().overlapCount());
	}

static size_t ice_particleDeadFind(std::vector<SnowflakeModel::IceParticle>& ice_particles)
	{
	auto i=ice_particles.begin();
	while(i!=ice_particles.end())
		{
		if(i->dead())
			{return i-ice_particles.begin();}
		++i;
		}
	return i-ice_particles.begin();
	}

bool Simstate::step()
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();
	if(m_data.frame%64==0)
		{
		fprintf(stderr,"\r# %.3g%% done. Cs: %zu. Cr: %zu. B: %zu. M: %zu. D: %zu. Matrix Fill ratio %.3g%%   "
			,progressGet()*100
			,m_data.collisions
			,m_data.collisions_rejected
			,m_data.births
			,m_data.melts
			,m_data.dropouts
			,100*static_cast<double>(m_data.N_particles)/ice_particles.size());
		}

	auto pair_merge=ice_particlesChoose(C_mat,randgen);
	m_data.tau+=-log(U(0.0,1.0,randgen))/double(C_mat.sumGetMt());

	if(pair_merge.first==pair_merge.second)
		{
		auto k=ice_particleDeadFind(ice_particles);
		if(k==ice_particles.size())
			{
		//TODO reallocate matrix? 
			fprintf(stderr,"\n# Event rejected %zu: Cloud is full %zu %zu"
				,m_data.frame,m_data.N_particles,k);
			return 0;
			}
		else
			{
			ice_particles[k]=ice_particlePrepare(r_setup,randgen);
			++m_data.N_particles;
			++m_data.frame;
			++m_data.births;
			matrixRowUpdate(k,ice_particles,C_mat,r_setup.m_data,m_data.N_particles);
			return 1;
			}
		}
	else
	if(pair_merge.first==ice_particles.size())
		{
		auto k=pair_merge.second;
		auto name_prefix=r_setup.m_output_dir + "/mesh-dropped-";
		particleDump(ice_particles[k],name_prefix.c_str(),m_data.frame+1);
		if(dropped_stats!=nullptr)
			{
			dropped_stats->printf("%zu\t%zu\t%.15g\t",m_data.frame+1,time(nullptr)
				,m_data.tau);
			particleDumpStats(ice_particles[k],*dropped_stats);
			}
		ice_particles[k].kill();
		--m_data.N_particles;
		++m_data.N_particles_dropped;
		++m_data.dropouts;
		matrixRowUpdate(k,ice_particles,C_mat,r_setup.m_data,m_data.N_particles);
		++m_data.frame;
		return 1;
		}
	else
	if(pair_merge.second==ice_particles.size())
		{
		auto k=pair_merge.first;
		ice_particles[k].kill();
		--m_data.N_particles;
		++m_data.frame;
		++m_data.melts;
		matrixRowUpdate(k,ice_particles,C_mat,r_setup.m_data,m_data.N_particles);
		return 1;
		}
	else
		{
		auto& g_b=ice_particles[pair_merge.first];
		auto& s_b=g_b.solidGet();
		s_b.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));

		auto& g_a=ice_particles[pair_merge.second];
		auto s_a=g_a.solidGet();
		s_a.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));
		double vol_overlap=0;
		size_t overlap_count=0;
		auto retry_count=r_setup.m_data.m_merge_retries;
		do
			{
			auto f_b=faceChoose(s_b,randgen);
			auto coords=drawFromTriangle(randgen);
			auto u=f_b.vertexGet(0)
				+ coords.x*(f_b.vertexGet(1) - f_b.vertexGet(0))
				+ coords.y*(f_b.vertexGet(2) - f_b.vertexGet(0));

			auto f_a=faceChoose(s_a,randgen);
			coords=drawFromTriangle(randgen);
			auto v=f_a.vertexGet(0)
				+ coords.x*(f_a.vertexGet(1) - f_a.vertexGet(0))
				+ coords.y*(f_a.vertexGet(2) - f_a.vertexGet(0));


			auto R=SnowflakeModel::vectorsAlign(f_a.m_normal, -f_b.m_normal);

			auto pos=SnowflakeModel::Vector(u - R.first*v);

			SnowflakeModel::Matrix T;
			T=glm::translate(T,pos);
			SnowflakeModel::Matrix R_x;
			R_x=glm::translate(R_x,SnowflakeModel::Vector(v));
			R_x=glm::rotate(R_x,2*glm::pi<float>()*float(U_rot(randgen))
				/(U_rot.max()+1),f_a.m_normal);
			R_x=glm::translate(R_x,SnowflakeModel::Vector(-v));
			s_a.transform(T*R.first*R_x,R.second);
			overlap_count=overlap(s_b,s_a,r_setup.m_data.m_overlap_max,vol_overlap);

			auto N_a=s_a.subvolumesCount();
			auto N_b=s_b.subvolumesCount();
			auto overlap_max=std::min(N_a,N_b);
			auto overlap_min=
				std::min(N_a + N_b - 2,static_cast<size_t>(r_setup.m_data.m_overlap_min*overlap_max));
			if(overlap_count>=overlap_min && overlap_count<=overlap_max * r_setup.m_data.m_overlap_max)
				{break;}
			else
				{
				if(retry_count==0)
					{
				#ifndef NDEBUG
					fprintf(stderr,"# Event %zu rejected: Overlap\n",m_data.frame);
				#endif
					++m_data.collisions_rejected;
					return 0;
					}
				--retry_count;
				}
			}
		while(1);
		++m_data.collisions;
		s_b.merge(s_a,vol_overlap,overlap_count);
		g_b.velocitySet(vTermCompute(s_b,r_setup)*randomDirection(randgen));
		g_a.kill();
		--m_data.N_particles;
		++m_data.frame;
		matrixRowUpdate(pair_merge.first,ice_particles,C_mat,r_setup.m_data
			,m_data.N_particles);
		matrixRowUpdate(pair_merge.second,ice_particles,C_mat,r_setup.m_data
			,m_data.N_particles);
		return 1;
		}
	return 0;
	}

class MonitorIterations:public SimstateMonitor
	{
	public:
		MonitorIterations(size_t N_iter):m_N_iter(N_iter)
			{}

		double progressGet(const Simstate& state) noexcept
			{
			return static_cast<double>(state.frameCurrentGet())
				/m_N_iter;
			}

	private:
		size_t m_N_iter;
	};

static std::unique_ptr<SimstateMonitor> iterations_check(const char* arg)
	{return std::unique_ptr<SimstateMonitor>( new MonitorIterations(atoi(arg)) );}


class MonitorSubvolsMaxCheck:public SimstateMonitor
	{
	public:
		MonitorSubvolsMaxCheck(size_t max):m_max(max)
			{}

		double progressGet(const Simstate& state) noexcept
			{
			auto particles_begin=state.particlesBegin();
			auto particles_end=state.particlesEnd();
			size_t subvols_max=0;
			while(particles_begin!=particles_end)
				{
				if(!particles_begin->dead())
					{
					auto& solid=particles_begin->solidGet();
					subvols_max=std::max(solid.subvolumesCount(),subvols_max);
					}
				++particles_begin;
				}
			return static_cast<double>(subvols_max)/m_max;
			}

	private:
		size_t m_max;
	};

static std::unique_ptr<SimstateMonitor> subvols_max_check(const char* arg)
	{return std::unique_ptr<SimstateMonitor>( new MonitorSubvolsMaxCheck(atoi(arg)) );}


class MonitorDMaxMaxCheck:public SimstateMonitor
	{
	public:
		MonitorDMaxMaxCheck(double max):m_max(max)
			{}

		double progressGet(const Simstate& state) noexcept
			{
			auto particles_begin=state.particlesBegin();
			auto particles_end=state.particlesEnd();
			float d_max_max=0.0f;
			while(particles_begin!=particles_end)
				{
				if(!particles_begin->dead())
					{
					auto& solid=particles_begin->solidGet();
					auto extrema=solid.extremaGet();
					auto d_max=glm::distance(extrema.first,extrema.second);
					d_max_max=std::max(d_max,d_max_max);
					}
				++particles_begin;
				}
			return d_max_max/m_max;
			}

	private:
		double m_max;
	};

static std::unique_ptr<SimstateMonitor> d_max_max_check(const char* arg)
	{return std::unique_ptr<SimstateMonitor>( new MonitorDMaxMaxCheck(atof(arg)) );}


class MonitorNDroppedCheck:public SimstateMonitor
	{
	public:
		MonitorNDroppedCheck(size_t max):m_max(max)
			{}

		double progressGet(const Simstate& state) noexcept
			{
			return static_cast<double>(state.nDroppedGet())/m_max;
			}

	private:
		size_t m_max;
	};

static std::unique_ptr<SimstateMonitor> n_dropped_check(const char* arg)
	{return std::unique_ptr<SimstateMonitor>( new MonitorNDroppedCheck(atoll(arg)) );}




class MonitorVMaxCheck:public SimstateMonitor
	{
	public:
		MonitorVMaxCheck(double max):m_max(max)
			{}

		double progressGet(const Simstate& state) noexcept
			{
			auto particles_begin=state.particlesBegin();
			auto particles_end=state.particlesEnd();
			float d_max_max=0.0f;
			while(particles_begin!=particles_end)
				{
				if(!particles_begin->dead())
					{
					auto& solid=particles_begin->solidGet();
					d_max_max=std::max(solid.volumeGet(),d_max_max);
					}
				++particles_begin;
				}
			return d_max_max/m_max;
			}

	private:
		double m_max;
	};

static std::unique_ptr<SimstateMonitor> v_max_check(const char* arg)
	{return std::unique_ptr<SimstateMonitor>( new MonitorVMaxCheck(atof(arg)) );}



class MonitorInfinity:public SimstateMonitor
	{
	public:
		MonitorInfinity()
			{}

		double progressGet(const Simstate& state) noexcept
			{
			return 0.0;
			}

	private:
		size_t m_N_iter;
	};

static std::unique_ptr<SimstateMonitor> infinity_check(const char* arg)
	{return std::unique_ptr<SimstateMonitor>( new MonitorInfinity );}


static std::unique_ptr<SimstateMonitor> bad_condition(const char* arg)
	{throw "Unknown stop condition";}


int main(int argc,char** argv)
	{
	try
		{
		Setup setup(argc,argv);
		if(setup.m_data.m_actions&Setup::HELP_SHOW)
			{
			helpShow();
			return 0;
			}

		if(setup.m_statefile_in.size())
			{
			SnowflakeModel::DataDump dump(setup.m_statefile_in.c_str()
				,SnowflakeModel::DataDump::IOMode::READ);
			auto statefile=setup.m_statefile_in;
			auto file_out=setup.m_statefile_out; //Save value
			setup=Setup(dump);
			setup.m_statefile_out=file_out; //Restore value
			setup.m_statefile_in=statefile;
			}
		setup.validate();

		if(setup.m_data.m_actions&Setup::PARAM_SHOW)
			{
			setup.m_prototypes.solidsEnum([](const char* name,const SnowflakeModel::Solid& solid)
				{
				auto& params=solid.deformationTemplatesGet();
				auto ptr=params.data();
				auto end=ptr+params.size();
				printf("Parameters for %s are:\n",name);
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
				});
			return 0;
			}


		fprintf(stderr,"# Initializing\n");
		std::map<std::string,SimstateMonitor::Factory> monitor_selector;
		monitor_selector["iterations"]=iterations_check;
		monitor_selector["subvols_max"]=subvols_max_check;
		monitor_selector["infinity"]=infinity_check;
		monitor_selector["d_max_max"]=d_max_max_check;
		monitor_selector["v_max"]=v_max_check;
		monitor_selector["n_dropped_max"]=n_dropped_check;

		auto monitor=monitor_selector[setup.m_stopcond_name];
		monitor=(monitor==nullptr)?bad_condition:monitor;
		std::unique_ptr<Simstate> state;
		if(setup.m_statefile_in.size()==0)
			{
			state.reset(new Simstate(setup,monitor(setup.m_stopcond_arg.c_str())));
			}
		else
			{
			state.reset(new Simstate(setup,SnowflakeModel::DataDump(setup.m_statefile_in.c_str(),SnowflakeModel::DataDump::IOMode::READ)
				,monitor(setup.m_stopcond_arg.c_str())));
			}
		auto now=SnowflakeModel::getdate();
		setup.paramsDump();
		fflush(stdout);
		fprintf(stderr,"# Simulation started at %s\n",now.c_str());
		SnowflakeModel::CtrlCHandler stopper;
		state->statsDump();
		while(state->progressGet()<1.0 && !stopper.captured())
			{
			if(state->step())
				{state->statsDump();}
			}
		fprintf(stderr,"\n# Exiting\n");

		if(setup.m_statefile_out.size()!=0)
			{
			auto filename_dump=setup.m_statefile_out;
			fprintf(stderr,"# Dumping simulation state to %s\n",filename_dump.c_str());
			SnowflakeModel::DataDump dump(filename_dump.c_str()
				,SnowflakeModel::DataDump::IOMode::WRITE);
			setup.write(dump);
			state->write(dump);
			}

		state->statsDump(1);

		if(setup.m_data.m_actions&Setup::GEOMETRY_DUMP)
			{
			fprintf(stderr,"# Dumping wavefront files\n");
			state->geometryDump();
			}

		if(setup.m_data.m_actions&Setup::GEOMETRY_DUMP_ICE)
			{
			fprintf(stderr,"# Dumping crystal prototype files\n");
			state->prototypesDump();
			}

		if(setup.m_data.m_actions&Setup::GEOMETRY_SAMPLE)
			{
			fprintf(stderr,"# Dumping adda files\n");
			state->rasterize();
			}
		}
	catch(const char* message)
		{
		fprintf(stderr,"# Error: %s\n",message);
		return -1;
		}
	return 0;
	}
