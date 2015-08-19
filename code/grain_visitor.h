#ifdef __WAND__
target[name[grain_visitor.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_GRAINVISITOR_H
#define SNOWFLAKEMODEL_GRAINVISITOR_H

namespace SnowflakeModel
	{
	class AggregateEdge;
	class Aggregate;

	class GrainVisitor
		{
		public:
			virtual void branchBegin(AggregateEdge& edge,Aggregate& aggregate)=0;
			virtual void branchEnd(AggregateEdge& edge,Aggregate& aggregate)=0;
			virtual void grainProcess(AggregateEdge& edge,Aggregate& aggregate)=0;
		};
	}
	
#endif
