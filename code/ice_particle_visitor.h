#ifdef __WAND__
target[name[ice_particle_visitor.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_GRAINVISITOR_H
#define SNOWFLAKEMODEL_GRAINVISITOR_H

namespace SnowflakeModel
	{
	class AggregateEdge;
	class AggregateGraph;

	class IceParticleVisitor
		{
		public:
			virtual void branchBegin(AggregateEdge& edge,AggregateGraph& aggregate)=0;
			virtual void branchEnd(AggregateEdge& edge,AggregateGraph& aggregate)=0;
			virtual void iceParticleProcess(AggregateEdge& edge,AggregateGraph& aggregate)=0;
		};
	}

#endif
