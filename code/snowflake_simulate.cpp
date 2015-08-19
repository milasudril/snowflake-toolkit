#ifdef __WAND__
target[name[snowflake_simulate] type[application] platform[;GNU/Linux]]
#endif

#include "config_parser.h"
#include "solid_loader.h"
#include "list_indexed.h"
#include "solid_writer.h"
#include "solid.h"
#include "voxelbuilder_adda.h"
#include "file_out.h"
#include "grain.h"
#include "twins.h"

#include <getopt.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <map>
#include <cstdlib>
#include <string>
#include <random>


static constexpr float NU=8;
static constexpr float ETA=1;
static constexpr float RHO_A=0.0005;
static constexpr float RHO_P=1;
static constexpr float G=1;
static constexpr float TAU=1.5;
static constexpr float C_1=8.5;
static constexpr float C_2=0.1519;
static constexpr float N_=1000;
static constexpr float V_WORLD=1e3;

struct Setup
	{
	std::string m_crystal;
	std::string m_mesh_output;
	std::string m_geom_output;
	std::string m_stat_output;

	int size_x;
	int size_y;
	int size_z;
	bool help_show;

	uint32_t seed;
	size_t N;
	float V;
	float nu;
	float eta;
	float rho_a;
	float rho_p;
	float g;
	float tau;
	float c_1;
	float c_2;

	Setup(int argc,char** argv);
	void paramsDump();
	};

static const struct option PROGRAM_OPTIONS[]=
	{
		 {"shape",required_argument,nullptr,'a'}
		,{"help",no_argument,nullptr,'h'}
		,{"mesh-output",required_argument,nullptr,'m'}
		,{"sample-geometry",required_argument,nullptr,'s'}
		,{"stat-output",required_argument,nullptr,'b'}
		,{"seed",required_argument,nullptr,'S'}

		,{"N",required_argument,nullptr,'N'}
		,{"V",required_argument,nullptr,'V'}
		,{"nu",required_argument,nullptr,'n'}
		,{"eta",required_argument,nullptr,'e'}
		,{"rho_a",required_argument,nullptr,'x'}
		,{"rho_p",required_argument,nullptr,'y'}
		,{"g",required_argument,nullptr,'g'}
		,{"tau",required_argument,nullptr,'t'}
		,{"C_1",required_argument,nullptr,'c'}
		,{"C_2",required_argument,nullptr,'d'}
		,{0,0,0,0}
	};

Setup::Setup(int argc,char** argv):
	size_x(1),size_y(1),size_z(1),help_show(0)
	{
	int option_index;
	int c;
	const char* sample_geometry=nullptr;
	const char* crystal=nullptr;
	seed=time(0);

	nu=NU;
	eta=ETA;
	rho_a=RHO_A;
	rho_p=RHO_P;
	g=G;
	tau=TAU;
	c_1=C_1;
	c_2=C_2;
	N=N_;
	V=V_WORLD;


	while( (c=getopt_long(argc,argv,"",PROGRAM_OPTIONS,&option_index))!=-1)
		{
		switch(c)
			{
			case 'h':
				help_show=1;
				break;
			case 'a':
				crystal=optarg;
				m_crystal=optarg;
				break;
			case 'm':
				m_mesh_output=optarg;
				break;
			case 's':
				sample_geometry=optarg;
				break;
			case 'S':
				seed=atoi(optarg);
				break;

			case 'n':
				nu=atof(optarg);
				break;
			case 'e':
				eta=atof(optarg);
				break;
			case 'x':
				rho_a=atof(optarg);
				break;
			case 'y':
				rho_p=atof(optarg);
				break;
			case 'g':
				g=atof(optarg);
				break;
			case 't':
				tau=atof(optarg);
				break;
			case 'c':
				c_1=atof(optarg);
				break;
			case 'd':
				c_2=atof(optarg);
				break;

			case 'N':
				N=atoi(optarg);
				break;

			case 'V':
				V=atof(optarg);
				break;

			case 'b':
				m_stat_output=optarg;
				break;

			case '?':
				printf("%s\n",optarg);
				throw "Invalid parameter given";
			}
		}
	if(crystal==nullptr && !help_show)
		{
		throw "Crustaln file is not given. "
			"Try --help for more information.";
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
			m_geom_output="/dev/stdout";
			size_z=atoi(temp.data());
			}
		else
			{m_geom_output=sample_geometry;}
		}
	}

void Setup::paramsDump()
	{
	printf("Parameters:\n\n"
		"Shape: %s\n"
		"Seed:  %u\n"
		"N:     %zu\n"
		"V:     %.7g\n"
		"nu:    %.7g\n"
		"eta:   %.7g\n"
		"rho_a: %.7g\n"
		"rho_p: %.7g\n"
		"g:     %.7g\n"
		"tau:   %.7g\n"
		"C_1:   %.7g\n"
		"C_2:   %.7g\n"
		,m_crystal.data(),seed,N,V,nu,eta,rho_a,rho_p,g,tau,c_1,c_2);
	}

void helpShow()
	{
	printf("Options:\n\n"
		"--shape=crystal_file\n"
		"    Generate data using the shape stored in crystal_file."
		" See the reference manal for information about how to create "
		"such a file.\n\n"
		"--help\n"
		"    Print this text and exit.\n\n"
		"--stat-output=stat_file\n"
		"    Write resulting particle size statistics to stat_file\n"
		"--mesh-output=mesh_file_prefix\n"
		"    Export the output geometry as Wavefront files, that can "
		"be imported into 3D modelling software such as Blender.\n\n"
		"--sample-geometry=Nx,Ny,Nz,adda_file_prefix\n"
		"    Sample the output geometry to a grid of size Nx x Ny x Nz"
		", and store the output to adda_file\n\n"
		"--seed=integer\n"
		"    Sets the random number genererator seed\n\n"
		"--N=integer\n"
		"    The initial number of crystals\n\n"
		"--V=value\n"
		"    Size of the world\n\n"
		"--nu=value\n"
		"    Model parameter\n\n"
		"--eta=value\n"
		"    Model parameter\n\n"
		"--rho_a=value\n"
		"    Atmosphere density\n\n"
		"--rho_p=value\n"
		"    Particle density\n\n"
		"--g=value\n"
		"    Gravity acceleration\n\n"
		"--tau=valuen\n"
		"    Simulated time\n\n"
		"--C_1=value\n"
		"    Model parameter\n\n"
		"--C_2=value\n"
		"    Model parameter\n\n");
	}

template<class T>
T U(T a,T b,std::mt19937& randgen)
	{
	return std::uniform_int_distribution<T>(a,b)(randgen);
	}

float U(float a,float b,std::mt19937& randgen)
	{
	return std::uniform_real_distribution<float>(a,b)(randgen);
	}

SnowflakeModel::Vector randomDirection(std::mt19937& randgen)
	{
	auto theta=U(0,2*glm::pi<float>(),randgen);
	auto phi=U(0,glm::pi<float>(),randgen);

	return {cos(theta)*sin(phi),sin(theta)*sin(phi),cos(phi)};
	}

float vTermCompute(const SnowflakeModel::Solid& vol
	,const Setup& setup)
	{
	auto R=vol.rMaxGet();
	auto A=glm::pi<float>()*R*R;
	auto m=vol.volumeGet()*setup.rho_p;
	auto bb=vol.boundingBoxGet();
	R=(bb.m_max.x - bb.m_min.x)*(bb.m_max.y - bb.m_min.y);

	auto A_e=R*glm::pi<float>();
	auto X=8*m*setup.g*setup.rho_a*pow(A/A_e,1.0/4)
		/(glm::pi<float>()*setup.eta*setup.eta);
	auto Re=C_1*sqrt( (sqrt(1+C_2*sqrt(X))-1) );
	auto ret=Re*setup.eta*sqrt(glm::pi<float>()/A)/(2*setup.rho_a);
	assert(!isnan(ret));
	return ret;
	}

SnowflakeModel::Grain grainPrepare(const SnowflakeModel::Solid& v_in
	,const Setup& setup,std::mt19937& randgen)
	{
	SnowflakeModel::Grain grain;
	grain.solidSet(v_in);

	float alpha=1+setup.nu;
	float beta=1+setup.nu;
	float k=alpha;
	float theta=1/beta;

	std::gamma_distribution<float> G(k,theta);

	auto s=G(randgen);

	grain.solidScale(s);

	grain.densitySet(setup.rho_p);
	auto vt=vTermCompute(grain.solidGet(),setup);
	grain.velocitySet(SnowflakeModel::Vector(0,0,vt));

	return std::move(grain);
	}

void init(const SnowflakeModel::Solid& v_in
	,const Setup& setup,std::mt19937& randgen
	,SnowflakeModel::ListIndexed<SnowflakeModel::Grain>& grains)
	{
	auto n=setup.N;
	while(n)
		{
		grains.append(grainPrepare(v_in,setup,randgen));
		--n;
		}
	}

float C(size_t k,size_t l
	,const SnowflakeModel::ListIndexed<SnowflakeModel::Grain>& grains
	,float V_world)
	{
	auto& g_k=grains[k];
	auto& S_k=g_k.solidGet();
	auto R_max_k=S_k.rMaxGet();
	auto v_k=g_k.velocityGet();

	auto& g_l=grains[l];
	auto& S_l=g_l.solidGet();
	auto R_max_l=S_l.rMaxGet();
	auto v_l=g_l.velocityGet();

	return glm::pi<float>()*(R_max_k + R_max_l)*(R_max_k + R_max_l)
		*glm::distance(v_k,v_l)/V_world;
	}

std::pair<SnowflakeModel::Twins<size_t>,float>
grainsChoose(const SnowflakeModel::ListIndexed<SnowflakeModel::Grain>& grains
	,float t
	,float V_world
	,std::mt19937& randgen)
	{
	auto N=grains.length();
	std::vector<float> C_k;
	C_k.reserve(N-1);
	float C_0=0;
	for(size_t k=0; k<N-1; ++k)
		{
		float C_l_sum=0;
		for(size_t l=k+1; l<N; ++l)
			{C_l_sum+=C(k,l,grains,V_world);}
		C_0+=C_l_sum;
		C_k.push_back(C_l_sum);
		}
	C_0/=V_world;

	printf("%.7g %.7g\n",t,C_0);


	float tau=-log(U(0.0f,1.0f,randgen))/C_0;

	std::discrete_distribution<size_t> P_ck(C_k.begin(),C_k.end());
	auto k=P_ck(randgen);

	std::vector<float> C_l;
	C_l.reserve(N-(k+1));
	for(size_t l=k+1; l<N; ++l)
		{C_l.push_back( C(k,l,grains,V_world) );}

	std::discrete_distribution<size_t> P_cl(C_l.begin(),C_l.end());
	auto l=P_cl(randgen)+k+1;

	return {{k,l},tau};
	}

const SnowflakeModel::VolumeConvex::Face& faceChoose
	(SnowflakeModel::Solid& s_a,std::mt19937& randgen)
	{
	auto& subvol_sel=s_a.subvolumeGet(U(0,s_a.subvolumesCount()-1,randgen));
	auto face_out_index=U(0,subvol_sel.facesOutCount()-1,randgen);
	auto& ret=subvol_sel.faceOutGet(face_out_index);
	subvol_sel.faceOutRemove(face_out_index);
	subvol_sel.facesMidpointCompute();
	subvol_sel.facesNormalCompute();
	return ret;
	}

int main(int argc,char** argv)
	{
	try
		{
		Setup setup(argc,argv);
		if(setup.help_show)
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

		std::mt19937 randgen(setup.seed);
	//	http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
		randgen.discard(65536);
		setup.paramsDump();
		fprintf(stderr,"Initializing\n");
		SnowflakeModel::ListIndexed<SnowflakeModel::Grain> grains;
		init(s_in,setup,randgen,grains);

		std::uniform_real_distribution<float> U_rot(0,2*glm::pi<float>());
		std::list<SnowflakeModel::Solid> solids_out;
		float tau_stop=1.5;
		float tau=0;
		fprintf(stderr,"Running simulation\n");
		while(grains.length()!=1 && tau < tau_stop)
			{
			auto tmp=grainsChoose(grains,tau,setup.V,randgen);
			tau+=tmp.second;
			auto pair_merge=tmp.first;
			auto& g_b=grains[pair_merge.first];
			auto s_b=g_b.solidGet();
			s_b.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));
			auto f_b=std::move(faceChoose(s_b,randgen));
			auto u=SnowflakeModel::Vector(f_b.m_mid);

			auto& g_a=grains[pair_merge.second];
			auto s_a=g_a.solidGet();
			s_a.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));
			auto f_a=std::move(faceChoose(s_a,randgen));
			auto v=SnowflakeModel::Vector(f_a.m_mid);


			auto R=SnowflakeModel::vectorsAlign(f_a.m_normal, -f_b.m_normal);

			auto pos=u - SnowflakeModel::Vector(R.first*SnowflakeModel::Point(v,1));

			SnowflakeModel::Matrix T;
			T=glm::translate(T,pos);
		//	T=glm::rotate(T,U_rot(randgen),f_a.m_normal);
		//	printf("v_a: %zu, v_b: %zu\n",v_a.subvolumesCount(),v_b.subvolumesCount());
			s_b.merge(T*R.first,s_a,R.second);
			solids_out.push_back(std::move(s_b));
			auto last=solids_out.end();
			--last;
			g_b.solidSet( *last );
			g_b.velocitySet(SnowflakeModel::Vector(0,0,vTermCompute(*last,setup)));
			grains.remove(pair_merge.second);
			}

		if(setup.m_stat_output!="")
			{
			fprintf(stderr,"Dumping r_max\n");
			SnowflakeModel::FileOut file_out(setup.m_stat_output.data());
			auto i=solids_out.begin();
			size_t count=1;
			while(i!=solids_out.end())
				{
				file_out.printf("%.7g\n",i->rMaxGet());
				++i;
				++count;
				}
			}

		if(setup.m_mesh_output!="")
			{
			fprintf(stderr,"Dumping wavefront files\n");
			auto i=solids_out.begin();
			size_t count=1;
			while(i!=solids_out.end())
				{
				char num_buff[16];
				sprintf(num_buff,"%zx.obj",count);
				SnowflakeModel::FileOut file_out(
					(setup.m_mesh_output+num_buff).data()
					);
				SnowflakeModel::SolidWriter writer(file_out);
				writer.write(*i);
				++i;
				++count;
				}
			}

		if(setup.m_geom_output!="")
			{
			fprintf(stderr,"Dumping adda files\n");
			auto i=solids_out.begin();
			size_t count=1;
			while(i!=solids_out.end())
				{
				char num_buff[16];
				sprintf(num_buff,"%zx.adda",count);
				SnowflakeModel::FileOut file_out(
					(setup.m_geom_output+num_buff).data()
					);
				SnowflakeModel::VoxelbuilderAdda builder(file_out
					,setup.size_x,setup.size_y,setup.size_z
					,i->boundingBoxGet());

				i->geometrySample(builder);
				++i;
				++count;
				}
			}
		}
	catch(const char* message)
		{
		fprintf(stderr,"Error: %s\n",message);
		return -1;
		}
	return 0;
	}
