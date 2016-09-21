//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"ice_particle.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "ice_particle.h"

using namespace SnowflakeModel;

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<IceParticle::Data>::fields[]=
		{
		 {"velocity",offsetOf(&IceParticle::Data::m_velocity),vectorObj().typeGet()}
		,{"density",offsetOf(&IceParticle::Data::m_density),DataDump::MetaObject<decltype(IceParticle::Data::m_density)>().typeGet()}
		,{"dead",offsetOf(&IceParticle::Data::m_dead),DataDump::MetaObject<decltype(IceParticle::Data::m_dead)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<IceParticle::Data>::field_count=3;
	}

void IceParticle::solidGenerate() const
	{
	if(m_flags_dirty&DEFORMATIONS_DIRTY)
		{
		m_deformations=r_solid->deformationTemplatesGet();
		m_flags_dirty&=~DEFORMATIONS_DIRTY;
		}
	m_solid_generated.clear();
	auto subvolume=r_solid->subvolumesBegin();
	while(subvolume!=r_solid->subvolumesEnd())
		{
		auto vc=*subvolume;
		auto deformation=m_deformations.data();
		auto deformations_end=deformation + m_deformations.size();
		while(deformation!=deformations_end)
			{
			if(*(deformation->nameGet().data())!='$')
				{
				vc.transformGroup(deformation->nameGet(),deformation->matrixGet());
				}
			else
			if(deformation->nameGet()=="$global")
				{vc.transform(deformation->matrixGet());}
			else
				{throw "Reserved group name";}
			++deformation;
			}
	//	There should not be any overlap here.
		m_solid_generated.subvolumeAdd(std::move(vc),0,0);
		++subvolume;
		}

	if(r_solid->mirrorFlagTest(Solid::MIRROR_HEADING))
		{
		auto bb=m_solid_generated.boundingBoxGet();
		Matrix trans;
		trans=glm::translate(trans,Vector(-bb.m_min.x,0,0));
		m_solid_generated.transform(trans,0);

		auto volume_copy=m_solid_generated;
		Matrix T;
		T[0][0]=-1;
		m_solid_generated.merge(T,volume_copy,1);
		}
	m_solid_generated.centerBoundingBoxAt(Point{0,0,0,1});
	m_flags_dirty&=~VOLUME_DIRTY;
	}

void IceParticle::parameterSet(const std::string& name,float value)
	{
	if(m_flags_dirty&DEFORMATIONS_DIRTY)
		{
		m_deformations=r_solid->deformationTemplatesGet();
		m_flags_dirty&=~DEFORMATIONS_DIRTY;
		}
	auto deformation=m_deformations.data();
	auto deformations_end=deformation+m_deformations.size();
	while(deformation!=deformations_end)
		{
		auto i=deformation->parameterFind(name);
		if(i!=nullptr)
			{
			auto index_current=i->data();
			auto index_end=index_current+i->size();
			while(index_current!=index_end)
				{
				(*deformation)[*index_current]=value;
				++index_current;
				}
			}
		++deformation;
		}
	m_flags_dirty|=VOLUME_DIRTY;
	}

void IceParticle::solidScale(float c)
	{
	if(m_flags_dirty&VOLUME_DIRTY)
		{solidGenerate();}

	Matrix S;
	S=glm::scale(S,Vector(c,c,c));
	m_solid_generated.transform(S,c<0);
	}

IceParticle::IceParticle(const DataDump& dump,const char* name)
	{
	auto group=dump.groupOpen(name);
	std::string path(name);
	m_data=dump.arrayGet<Data>((path + "/data").c_str() ).at(0);

		{
		auto defgroup_name=path + "/deformations";
		auto defgroup=dump.groupOpen(defgroup_name.c_str());
		defgroup_name+='/';
		
		dump.iterate(*defgroup,[&dump,&defgroup_name,this]
			(const char* group_name)
			{
			auto group_name_current=defgroup_name + group_name;
			m_deformations.push_back(SolidDeformation(dump,group_name_current.c_str()));
			});
		}

	m_solid_generated=Solid(dump,(path+"/solid_generated").c_str());
	m_flags_dirty=0;
	r_solid=nullptr;
	}

void IceParticle::write(const char* id,DataDump& dump) const
	{
	auto group=dump.groupCreate(id);
	std::string path(id);
	dump.write((path + "/data").c_str(),&m_data,1);

		{
		auto deformation_group=path + "/deformations";
		auto deformations=dump.groupCreate(deformation_group.c_str());
		auto deformation=m_deformations.data();
		auto deformations_end=deformation + m_deformations.size();
		deformation_group+='/';
		size_t k=0;
		while(deformation!=deformations_end)
			{
			char id[32];
			sprintf(id,"%016zx",k);
			auto def_name=deformation_group + id;
			deformation->write(def_name.c_str(),dump);
			++deformation;
			++k;
			}
		}

		m_solid_generated.write((path+"/solid_generated").c_str(),dump);
	}

