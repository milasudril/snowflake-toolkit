//@ {"targets":[{"name":"bvhtree.h","type":"include"}]}

#ifndef SNOWFLAKEMODEL_BVHTREE_H
#define SNOWFLAKEMODEL_BVHTREE_H

#include "bounding_box.h"
#include <list>

namespace SnowflakeModel
	{
	template<class Value>
	class BVHTree
		{
		public:
			BVHTree(const BoundingBox& boundaries);

			Node& boxFind(const BoundingBox& bb);
			void remove(Node* node);
			void insert(const BoundingBox& bb,const Value& v);

		private:
			struct Node
				{
				Node* parent;
				BoundingBox bb;
				Node* children[8];
				BoundingBox bb;
				std::list<Value> content;
				};
			Node m_root;
			

			Node& boxFindImpl(const Node& root,const BoundingBox& bb);
		};

	template<class T>
	BVHTree<T>::Node* boxFindImpl(const Node& root,const BoundingBox& bb)
		{
		if( /* Terminate */)
			{
			return root;
			}
		for(int k=0;k<8;++k)
			{
			auto child=*( root.children[k] ) ;
			if(bb.inside(child->bb))
				{return boxFindImpl(child,bb);}
				
			}
		}
	}


#endif