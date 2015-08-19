#ifdef __WAND__
target[name[grain.h] type[include]]
dependency[grain.o]
#endif

#ifndef SNOWFLAKEMODEL_GRAIN_H
#define SNOWFLAKEMODEL_GRAIN_H

#include "solid.h"

namespace SnowflakeModel
	{
	class Grain
		{
		public:
			Grain():m_flags_dirty(0),m_dead(0)
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

		private:
			const Solid* r_solid;
			mutable std::vector<SolidDeformation> m_deformations;
			mutable Solid m_solid_generated;
			mutable uint32_t m_flags_dirty;
			static constexpr uint32_t DEFORMATIONS_DIRTY=0x1;
			static constexpr uint32_t VOLUME_DIRTY=0x2;


			SnowflakeModel::Vector m_velocity;
			float m_density;
			float m_tau_born;
			bool m_dead;

			void solidGenerate() const;
		};
	}

#endif
