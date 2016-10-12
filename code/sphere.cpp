//@	{"targets":[{"name":"sphere.o","type":"object"}]}

#include "sphere.h"
#include "grid.h"
#include <stack>

using namespace SnowflakeModel;

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<Sphere>::fields[]=
		{
		 {"location",offsetOf(&Sphere::m_location),pointObj().typeGet()}
		,{"radius",offsetOf(&Sphere::m_radius),DataDump::MetaObject<decltype(Sphere::m_radius)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Sphere>::field_count=2;
	}

void Sphere::geometrySample(Grid& grid) const
	{
	std::stack<PointInt> nodes;

	nodes.push(grid.quantize(midpointGet()));
	const auto bb=boundingBoxGet();
	grid.bitAnd(grid.quantize(bb.m_min),grid.quantize(bb.m_max),0xf0);
	while(nodes.size()!=0)
		{
		auto node_current=nodes.top();
		nodes.pop();
		auto offset=grid.offsetGet(node_current);
		if(!grid.cellFilled(offset,0xf))
			{
			if(inside(grid.dequantize(node_current) ) )
				{
				grid.cellFill(offset,0xff);
				nodes.push(node_current+PointInt(-1,0,0,0));
				nodes.push(node_current+PointInt(1 ,0,0,0));
				nodes.push(node_current+PointInt(0,-1,0,0));
				nodes.push(node_current+PointInt(0,1, 0,0));
				nodes.push(node_current+PointInt(0,0,-1,0));
				nodes.push(node_current+PointInt(0,0, 1,0));
				}
			}
		}
	}
