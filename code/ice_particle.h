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
#include "datadump.h"

namespace SnowflakeModel
	{
	class IceParticle
		{
		public:
			IceParticle():m_flags_dirty(0),m_dead(0)
				{}

			void solidSet(const Solid& solid)
				{
				r_solid=&solid;
				m_flags_dirty|=VOLUME_DIRTY|DEFORMATIONS_DIRTY;
				m_dead=0;
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
				{return m_velocity;}

			void velocitySet(const Vector& v)
				{m_velocity=v;}

			float densityGet() const
				{return m_density;}

			void densitySet(float density)
				{m_density=density;}

			void kill()
				{m_dead=1;}

			bool dead() const
				{return m_dead;}

			void write(const char* id,DataDump& dump) const;

		private:
			const Solid* r_solid;
			mutable std::vector<SolidDeformation> m_deformations; //Saved
			mutable Solid m_solid_generated; //Saved
			mutable uint32_t m_flags_dirty;
			static constexpr uint32_t DEFORMATIONS_DIRTY=0x1;
			static constexpr uint32_t VOLUME_DIRTY=0x2;


			Vector m_velocity; //Saved
			float m_density; //Saved
			bool m_dead; //Saved

			void solidGenerate() const;

			friend class DataDump::MetaObject<IceParticle>;
		};
	}

#endif
