//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"ice_particle.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"ice_particle.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_ICEPARTICLE_H
#define SNOWFLAKEMODEL_ICEPARTICLE_H

#include "solid.h"

namespace SnowflakeModel
	{
	class IceParticle
		{
		public:
			struct Data
				{
				Vector m_velocity;
				float m_density;
				uint32_t m_dead;
				};

			IceParticle(const DataDump& dump,const char* name);

			IceParticle():r_solid(nullptr),m_flags_dirty(0)
				,m_data{{0.0f,0.0f,0.0f},1.0f,1}
				{
				}

			void solidSet(const Solid& solid)
				{
				r_solid=&solid;
				m_flags_dirty|=VOLUME_DIRTY|DEFORMATIONS_DIRTY;
				m_data.m_dead=0;
				}

			const Solid& solidGet() const
				{
				if(m_flags_dirty&VOLUME_DIRTY)
					{solidGenerate();}
				return m_solid_generated;
				}

			Solid& solidGet()
				{
				if(m_flags_dirty&VOLUME_DIRTY)
					{solidGenerate();}
				return m_solid_generated;
				}

			void parameterSet(const std::string& name,float value);

			void solidScale(float c);

			const Vector& velocityGet() const
				{return m_data.m_velocity;}

			void velocitySet(const Vector& v)
				{m_data.m_velocity=v;}

			float densityGet() const
				{return m_data.m_density;}

			void densitySet(float density)
				{m_data.m_density=density;}

			void kill()
				{m_data.m_dead=1;}

			bool dead() const
				{return m_data.m_dead;}

			void write(const char* id,DataDump& dump) const;

		private:
			const Solid* r_solid;
			mutable std::vector<SolidDeformation> m_deformations; //Saved
			mutable Solid m_solid_generated; //Saved
			mutable uint32_t m_flags_dirty;
			static constexpr uint32_t DEFORMATIONS_DIRTY=0x1;
			static constexpr uint32_t VOLUME_DIRTY=0x2;

			Data m_data;

			void solidGenerate() const;


			friend class DataDump::MetaObject<Data>;
		};
	}

#endif
