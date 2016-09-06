//@	{
//@	"dependencies_extra":[],
//@		"targets":[
//@			{
//@			 "dependencies":[]
//@			,"name":"snowflake_simulate3"
//@			,"type":"application"
//@			,"cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@			}
//@		]
//@	}
#include "config_parser.h"
#include "solid_loader.h"
#include "solid_writer.h"
#include "solid_writer_prototype.h"
#include "solid.h"
#include "voxelbuilder_adda.h"
#include "file_out.h"
#include "ice_particle.h"
#include "twins.h"
#include "matrix_storage.h"
#include "element_randomizer.h"
#include "profile.h"
#include "ctrlchandler.h"
#include "getdate.h"
#include "filenameesc.h"
#include "datadump.h"

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
static constexpr char PARAM_NITER='M';
static constexpr char PARAM_PARAMSHOW='N';
static constexpr char PARAM_GEOMETRY_DUMP_ICE='O';


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
		,{"iterations",required_argument,nullptr,PARAM_NITER}
		,{"N",required_argument,nullptr,PARAM_PARTICLE_COUNT}
		,{"droprate",required_argument,nullptr,PARAM_DROPRATE}
		,{"growthrate",required_argument,nullptr,PARAM_GROWTHRATE}
		,{"meltrate",required_argument,nullptr,PARAM_MELTRATE}
		,{"param-show",no_argument,nullptr,PARAM_PARAMSHOW}
		,{0,0,0,0}
	};

struct DeformationData
	{
	std::string name;
	double mean;
	double standard_deviation;
	};

struct Setup
	{
	std::string m_crystal;
	std::vector<DeformationData> m_deformations;
	std::string m_output_dir;

	static constexpr size_t N=1023;
	static constexpr float DROPRATE=100;
	static constexpr float GROWTHRATE=100;
	static constexpr float MELTRATE=100;
	static constexpr size_t N_ITER=1024;

	static constexpr uint32_t HELP_SHOW=1;
	static constexpr uint32_t STATS_DUMP=2;
	static constexpr uint32_t GEOMETRY_DUMP=4;
	static constexpr uint32_t GEOMETRY_SAMPLE=8;
	static constexpr uint32_t GEOMETRY_DUMP_ICE=32;
	static constexpr uint32_t PARAM_SHOW=16;

	uint32_t m_actions;
	uint32_t m_stat_saverate;

	int m_size_x;
	int m_size_y;
	int m_size_z;

	size_t m_N;
	size_t m_N_iter;
	uint32_t m_seed;
	float m_droprate;
	float m_growthrate;
	float m_meltrate;

	Setup(int argc,char** argv);
	void paramsDump();
	void write(SnowflakeModel::DataDump& dump);
	};

Setup::Setup(int argc,char** argv):
	m_actions(0),m_size_x(1),m_size_y(0),m_size_z(0)
	{
	int option_index;
	int c;
	const char* sample_geometry=nullptr;
	std::vector<std::string> deformations;
	m_seed=time(nullptr);

	m_N=N;
	m_droprate=DROPRATE;
	m_growthrate=GROWTHRATE;
	m_meltrate=MELTRATE;
	m_N_iter=N_ITER;
	m_stat_saverate=256;

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

			case PARAM_OUTPUT_DIR:
				m_output_dir=optarg;
				break;

			case PARAM_GEOMETRY_SAMPLE:
			//	This option takes sub-options.
				sample_geometry=optarg;
				m_actions|=GEOMETRY_SAMPLE;
				break;

			case PARAM_GEOMETRY_DUMP:
				m_actions|=GEOMETRY_DUMP;
				break;

			case PARAM_GEOMETRY_DUMP_ICE:
				m_actions|=GEOMETRY_DUMP_ICE;
				break;

			case PARAM_STATS_DUMP:
				{
				m_actions|=STATS_DUMP;
				auto freq=atoi(optarg);
				if(freq!=0)
					{m_stat_saverate=freq;}
				}
				break;

			case PARAM_SEED:
				m_seed=atoi(optarg);
				break;

			case PARAM_PARTICLE_COUNT:
				m_N=atoi(optarg);
				break;

			case PARAM_DROPRATE:
				m_droprate=atof(optarg);
				break;

			case PARAM_GROWTHRATE:
				m_growthrate=atof(optarg);
				break;

			case PARAM_MELTRATE:
				m_meltrate=atof(optarg);
				break;

			case PARAM_NITER:
				m_N_iter=atoi(optarg);
				break;

			case PARAM_PARAMSHOW:
				m_actions|=PARAM_SHOW;
				break;

			case '?':
				throw "Invalid parameter given";

			}
		}

	if(m_growthrate<1e-7)
		{throw "Growthrate must be non-zero";}

	if(m_crystal=="" && !(m_actions&HELP_SHOW))
		{
		throw "Crystal file is not given. "
			"Try --help for more information.";
		}

	if(deformations.size()==0 && !( (m_actions&HELP_SHOW) || (m_actions&PARAM_SHOW)))
		{
		throw "No deformation is given. "
			"Try --param-show together with the chosen crystal file for more "
			"information.";
		}

	if( m_output_dir==""
		&& (m_actions&GEOMETRY_DUMP || m_actions&GEOMETRY_SAMPLE
			|| m_actions&STATS_DUMP))
		{throw "No output directory given";}

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
			{m_size_z=atoi(temp.data());}
		}

		{
		auto ptr=deformations.data();
		auto ptr_end=ptr+deformations.size();
		DeformationData deformation;
		deformation.mean=1;
		deformation.standard_deviation=0;
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
								deformation.mean=atof(str_temp.data());
								break;
							case 2:
								deformation.standard_deviation=atof(str_temp.data());
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
					deformation.mean=atof(str_temp.data());
					break;
				case 2:
					deformation.standard_deviation=atof(str_temp.data());
					break;
				}
			m_deformations.push_back(deformation);
			deformation.mean=1;
			deformation.standard_deviation=0;
			deformation.name.clear();
			++ptr;
			}
		}
	}

void Setup::paramsDump()
	{
	printf("Parameters:\n\n"
		"Shape:      %s\n"
		"Seed:       %u\n"
		"N:          %zu\n"
		"droprate:   %.7g\n"
		"growthrate: %.7g\n"
		"meltrate:   %.7g\n"
		,m_crystal.data()
		,m_seed,m_N,m_droprate,m_growthrate,m_meltrate);
	printf("\nDeformations:\n");
		{
		auto ptr=m_deformations.data();
		auto ptr_end=ptr+m_deformations.size();
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
		"--shape=crystal_file\n"
		"    Generate data using the shape stored in crystal_file."
		" See the reference manal for information about how to create "
		"such a file.\n\n"
		"--deformation=name,mean,std\n"
		"    Crystal file parameter to use for size modification. Use\n\n"
		"         --param-show\n\n"
		"    to list the name of available parameters.\n\n"
		"--help\n"
		"    Print this text and exit.\n\n"
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
		"files, so they can be used by other tools provided by the toolkit."
		"--sample-geometry=Nx,Ny,Nz\n"
		"    Sample the output geometry to a grid of size Nx x Ny x Nz\n\n"
		"--seed=integer\n"
		"--iterataions=integer\n"
		"    Sets the number of iterations\n\n"
		"    Sets the random number genererator seed\n\n"
		"--N=integer\n"
		"    The maximum number of crystals\n\n"
		"--sigma=value\n"
		"    Standard deviation in particle size\n\n"
		"--droprate=value\n"
		"--growthrate=value\n"
		"--meltrate=value\n"
		);
	}

void Setup::write(SnowflakeModel::DataDump& dump)
	{
	struct Trivial
		{};
	}




size_t U(size_t a,size_t b,std::mt19937& randgen)
	{
	return std::uniform_int_distribution<size_t>(a,b)(randgen);
	}

float U(float a,float b,std::mt19937& randgen)
	{
	return std::uniform_real_distribution<float>(a,b)(randgen);
	}

double U(double a,double b,std::mt19937& randgen)
	{
	return std::uniform_real_distribution<double>(a,b)(randgen);
	}

SnowflakeModel::Vector randomDirection(std::mt19937& randgen)
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

SnowflakeModel::IceParticle ice_particlePrepare(const SnowflakeModel::Solid& s_in
	,const Setup& setup,std::mt19937& randgen)
	{
	SnowflakeModel::IceParticle ice_particle;
	ice_particle.solidSet(s_in);

//	Setup parameters
		{
		auto deformation=setup.m_deformations.data();
		auto deformation_end=deformation+setup.m_deformations.size();
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
	,const Setup& setup)
	{
	auto& g_k=ice_particles[k];
	if(g_k.dead())
		{return 0;}

	if(setup.m_N==0)
		{abort();}

	auto v_k=g_k.velocityGet();
	return 2.0f*glm::length(v_k)*setup.m_droprate*pow(setup.m_N,-1.0f/3);
	}

float C_melt(size_t k,const std::vector<SnowflakeModel::IceParticle>& ice_particles
	,const Setup& setup)
	{
	auto& g_k=ice_particles[k];
	if(g_k.dead())
		{return 0;}

	return 2.0f*setup.m_meltrate/g_k.solidGet().volumeGet();
	}

void matrixRowUpdate(size_t k
	,const std::vector<SnowflakeModel::IceParticle>& ice_particles
	,SnowflakeModel::MatrixStorage& C_mat
	,const Setup& setup)
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
		*elem_current=C_drop(k,ice_particles,setup);
		++elem_current;
		++k;
		}
	C_mat(l,l)=0;
	}

void matrixDump(const SnowflakeModel::MatrixStorage& C_mat)
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

size_t ice_particleDeadFind(std::vector<SnowflakeModel::IceParticle>& ice_particles)
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

const SnowflakeModel::VolumeConvex::Face&
faceChoose(const SnowflakeModel::Solid& s_a,std::mt19937& randgen)
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
	auto& ret=subvol_sel.faceOutGet(face_out_index);
//	subvol_sel.faceOutRemove(face_out_index);
	subvol_sel.facesMidpointCompute();

	return ret;
	}

template<class T>
size_t randomDraw(const T* ptr_begin,const T* ptr_end
	,T p_mass,std::mt19937& randgen)
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

SnowflakeModel::Twins<size_t>
ice_particlesChoose(const SnowflakeModel::ElementRandomizer& randomizer
	,std::mt19937& randgen)
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();
	return randomizer.elementChoose(randgen);
	}

glm::vec2 drawFromTriangle(std::mt19937& randgen)
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
		Simstate(Setup&& setup,SnowflakeModel::Solid&& s_in)=delete;
		Simstate(Setup&& setup,const SnowflakeModel::Solid& s_in)=delete;
		Simstate(const Setup& setup,SnowflakeModel::Solid&& s_in)=delete;

		Simstate(const Setup& setup,const SnowflakeModel::Solid& s_in);
		~Simstate();

		bool step();
		void statsDump() const;
		void prototypesDump() const;

		double progressGet() const noexcept
			{return frame/static_cast<double>( r_setup.m_N_iter );}

		void geometryDump() const;

		void rasterize() const;

	private:
		const Setup& r_setup;
		const SnowflakeModel::Solid& r_s_in;
		double tau;
		size_t frame;
		SnowflakeModel::MatrixStorage C_mat;
		SnowflakeModel::ElementRandomizer randomizer;
		std::mt19937 randgen;
		std::vector<SnowflakeModel::IceParticle> ice_particles;
		std::vector<SnowflakeModel::IceParticle> ice_particles_dropped;
		size_t N_particles;
		SnowflakeModel::FileOut* frame_data_file;
		std::uniform_int_distribution<int> U_rot;
	};

Simstate::Simstate(const Setup& setup,const SnowflakeModel::Solid& s_in):
	 r_setup(setup),r_s_in(s_in),tau(0.0),frame(0),C_mat(setup.m_N+1,setup.m_N+1)
	,randomizer(C_mat),randgen(setup.m_seed),N_particles(0),frame_data_file(nullptr)
	,U_rot(0,5)
	{
//	http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
	randgen.discard(65536);

	auto n=setup.m_N;
	while(n)
		{
		ice_particles.push_back(ice_particlePrepare(s_in,setup,randgen));
		(ice_particles.end()-1)->kill();
		--n;
		}

	for(size_t k=0;k<ice_particles.size();++k)
		{matrixRowUpdate(k,ice_particles,C_mat,setup);}

	for(size_t k=0;k<ice_particles.size();++k)
		{
		C_mat(k,k)=2.0f*setup.m_growthrate/ice_particles.size();
		}

	if(setup.m_actions&Setup::STATS_DUMP)
		{
		mkdir(setup.m_output_dir.data(),S_IRWXU|S_IRGRP|S_IXGRP);
		frame_data_file=new SnowflakeModel::FileOut(
			(setup.m_output_dir+"/frame_data.txt").data()
			);

		frame_data_file->printf("# Frame\tClock time\tSimulation time\tN_cloud\tN_drop\tC_kl_sum\n");
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

		{
		auto i=ice_particles_dropped.begin();
		size_t count=0;
		while(i!=ice_particles_dropped.end())
			{
			char num_buff[32];
			sprintf(num_buff,"/mesh-dropped-%zx.obj",count);
			SnowflakeModel::FileOut file_out(
				(r_setup.m_output_dir+num_buff).data());
			SnowflakeModel::SolidWriter writer(file_out);
			writer.write(i->solidGet());
			++count;
			++i;
			}
		}
	}

void Simstate::prototypesDump() const
	{
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

		{
		auto i=ice_particles_dropped.begin();
		size_t count=0;
		while(i!=ice_particles_dropped.end())
			{
			char num_buff[32];
			sprintf(num_buff,"/mesh-dropped-%zx.obj",count);
			SnowflakeModel::FileOut file_out(
				(r_setup.m_output_dir+num_buff).data());
			SnowflakeModel::SolidWriter writer(file_out);
			writer.write(i->solidGet());
			++count;
			++i;
			}
		}

	}
	
Simstate::~Simstate()
	{
	if(frame_data_file!=nullptr)
		{delete frame_data_file;}
	}

void Simstate::rasterize() const
	{
		{
		auto i=ice_particles.begin();
		size_t count=0;
		while(i!=ice_particles.end())
			{
			if(!i->dead())
				{
				char num_buff[32];
				sprintf(num_buff,"/geom-%zx.adda",count);
				SnowflakeModel::FileOut file_out((r_setup.m_output_dir+num_buff).data());
				SnowflakeModel::VoxelbuilderAdda builder(file_out
					,r_setup.m_size_x,r_setup.m_size_y,r_setup.m_size_z
					,i->solidGet().boundingBoxGet());

				i->solidGet().geometrySample(builder);
				++count;
				}
			++i;
			}
		}

		{
		auto i=ice_particles_dropped.begin();
		size_t count=0;
		while(i!=ice_particles.end())
			{
			char num_buff[16];
			sprintf(num_buff,"/geom-dropped-%zx.adda",count);
			SnowflakeModel::FileOut file_out((r_setup.m_output_dir+num_buff).data());
			SnowflakeModel::VoxelbuilderAdda builder(file_out
				,r_setup.m_size_x,r_setup.m_size_y,r_setup.m_size_z
				,i->solidGet().boundingBoxGet());

			i->solidGet().geometrySample(builder);
			++count;
			++i;
			}
		}
	}

void Simstate::statsDump() const
	{
	if( !(frame_data_file!=nullptr && frame%r_setup.m_stat_saverate==0) )
		{return;}
	auto now=time(nullptr);

	double C_kl_sum=0;
		{
		auto elem_current=C_mat.rowGet(1);
		auto n=C_mat.nColsGet();
		size_t k=1;
		while(elem_current!=C_mat.rowGet(C_mat.nRowsGet() -1 ))
			{
			size_t l=0;
			while(l!=k)
				{
				C_kl_sum+=*elem_current;
				++l;
				++elem_current;
				}
			elem_current+=n-l;
			++k;
			}
			}

	//	Frame\tClock time\tSimulation time\tN_cloud\tN_drop\tC_kl_sum
		frame_data_file->printf(
			"%zu\t"
			"%zu\t"
			"%.15g\t"
			"%zu\t"
			"%zu\t"
			"%.15g\n",frame,now,tau,N_particles,ice_particles_dropped.size()
			,C_kl_sum);

			{
			char countbuff[32];
			sprintf(countbuff,"/frame-%zu.txt",frame);
			SnowflakeModel::FileOut file_out((r_setup.m_output_dir+countbuff).data());
			auto i=ice_particles.begin();
			file_out.printf("# R_max\tVolume\tSpeed\tL_x\tr_xy\tr_xz\tNumber of sub-volumes\n");
			while(i!=ice_particles.end())
				{
				if(!i->dead())
					{
					auto& bb=i->solidGet().boundingBoxGet();
					auto L=bb.m_max-bb.m_min;
					file_out.printf("%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%zu\n"
						,i->solidGet().rMaxGet()
						,i->solidGet().volumeGet()
						,glm::length(i->velocityGet())
						,L.x,L.x/L.y,L.x/L.z,i->solidGet().subvolumesCount());
					}
				++i;
				}
			}

			{
			char countbuff[32];
			sprintf(countbuff,"/frame-dropped-%zu.txt",frame);
			SnowflakeModel::FileOut file_out((r_setup.m_output_dir+countbuff).data());
			auto i=ice_particles_dropped.begin();
			file_out.printf("# R_max\tVolume\tSpeed\tL_x\tr_xy\tr_xz\tNumber of sub-volumes\n");
			while(i!=ice_particles_dropped.end())
				{
				auto& bb=i->solidGet().boundingBoxGet();
				auto L=bb.m_max-bb.m_min;
					file_out.printf("%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%.7g\t%zu\n"
						,i->solidGet().rMaxGet()
						,i->solidGet().volumeGet()
						,glm::length(i->velocityGet())
						,L.x,L.x/L.y,L.x/L.z,i->solidGet().subvolumesCount());
				++i;
				}
			}
	}

bool Simstate::step()
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();
	//TODO: frame/N_iter only works for fixed number of iterations
	if(frame%256==0)
		{fprintf(stderr,"\r# Running simulation. %.3g%% done.",progressGet()*100);}

	auto pair_merge=ice_particlesChoose(randomizer,randgen);
	tau+=-log(U(0.0,1.0,randgen))/double(C_mat.sumGetMt());

	if(pair_merge.first==pair_merge.second)
		{
		auto k=ice_particleDeadFind(ice_particles);
		if(k==ice_particles.size())
			{
			fprintf(stderr,"\n# Event rejected %zu: Cloud is full %zu %zu"
				,frame,N_particles,k);
			return 0;
			}
		else
			{
			ice_particles[k]=ice_particlePrepare(r_s_in,r_setup,randgen);
			++N_particles;
			matrixRowUpdate(k,ice_particles,C_mat,r_setup);
			++frame;
			return 1;
			}
		}
	else
	if(pair_merge.first==ice_particles.size())
		{
		auto k=pair_merge.second;
		ice_particles_dropped.push_back(ice_particles[k]);
		ice_particles[k].kill();
		--N_particles;
		matrixRowUpdate(k,ice_particles,C_mat,r_setup);
		++frame;
		return 1;
		}
	else
	if(pair_merge.second==ice_particles.size())
		{
		auto k=pair_merge.first;
		ice_particles[k].kill();
		--N_particles;
		matrixRowUpdate(k,ice_particles,C_mat,r_setup);
		++frame;
		return 1;
		}
	else
		{
		auto& g_b=ice_particles[pair_merge.first];
		auto& s_b=g_b.solidGet();
		s_b.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));
		auto f_b=faceChoose(s_b,randgen);
		auto coords=drawFromTriangle(randgen);
		auto u=f_b.vertexGet(0)
			+ coords.x*(f_b.vertexGet(1) - f_b.vertexGet(0))
			+ coords.y*(f_b.vertexGet(2) - f_b.vertexGet(0));

		auto& g_a=ice_particles[pair_merge.second];
		auto s_a=g_a.solidGet();
		s_a.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));
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
		if(!overlap(s_b,s_a))
			{
			s_b.merge(s_a);
			g_b.velocitySet(vTermCompute(s_b,r_setup)*randomDirection(randgen));
			g_a.kill();
			--N_particles;
			matrixRowUpdate(pair_merge.first,ice_particles,C_mat,r_setup);
			matrixRowUpdate(pair_merge.second,ice_particles,C_mat,r_setup);
			++frame;
			return 1;
			}
#ifndef NDEBUG
		else
			{
			fprintf(stderr,"# Event %zu rejected: Overlap\n",frame);
			}
#endif
		return 0;
		}
	return 0;
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

		SnowflakeModel::Solid s_in;
			{
			SnowflakeModel::FileIn file_in(setup.m_crystal.data());
			SnowflakeModel::ConfigParser parser(file_in);
			SnowflakeModel::SolidLoader loader(s_in);
			parser.commandsRead(loader);
			}

		if(setup.m_actions&Setup::PARAM_SHOW)
			{
			auto& params=s_in.deformationTemplatesGet();
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


		fprintf(stderr,"# Initializing\n");
		SnowflakeModel::CtrlCHandler stopper;
		setup.paramsDump();
		Simstate state(setup,s_in);
		fflush(stdout);
		auto now=SnowflakeModel::getdate();
		fprintf(stderr,"# Simulation started at %s\n",now.c_str());
		state.statsDump();
		while(state.progressGet()<1.0 && !stopper.captured())
			{
			if(state.step())
				{state.statsDump();}
			}
		fprintf(stderr,"\n# Exiting\n");
			{
			auto filename_dump=SnowflakeModel::filenameEscape(now.c_str());
			filename_dump+=".h5";
			fprintf(stderr,"# Dumping simulation state to %s\n",filename_dump.c_str());
			SnowflakeModel::DataDump dump(filename_dump.c_str());
			setup.write(dump);
			}

		state.statsDump();

		if(setup.m_actions&Setup::GEOMETRY_DUMP)
			{
			fprintf(stderr,"# Dumping wavefront files\n");
			state.geometryDump();
			}

		if(setup.m_actions&Setup::GEOMETRY_DUMP_ICE)
			{
			fprintf(stderr,"# Dumping crystal prototype files\n");
			state.prototypesDump();
			}

		if(setup.m_actions&Setup::GEOMETRY_SAMPLE)
			{
			fprintf(stderr,"# Dumping adda files\n");
			state.rasterize();
			}
		}
	catch(const char* message)
		{
		fprintf(stderr,"# Error: %s\n",message);
		return -1;
		}
	return 0;
	}
