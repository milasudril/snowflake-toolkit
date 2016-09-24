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
		struct Node
			{
			BoundingBox bb;
			std::list<Value> content;
			Node* children[8];
			};

		public:
			struct Reference
				{
				Node& node;
				typename std::list<Value>::const_iterator position;
				};

			BVHTree(const Vector& size,size_t depth);

			template<class Visitor>
			void nodesVisit(const BoundingBox& start,Visitor&& visitor) const noexcept;

			void remove(Reference& r) noexcept;

			Reference insert(const BoundingBox& bb,Value&& v);

		private:
			Node m_root;
			static Node& boxFindImpl(Node& root,const BoundingBox& bb) noexcept;

			template<class Visitor>
			static void nodesVisit(const Node& root,Visitor&& v) noexcept;
		};

	template<class Value>
	typename BVHTree<Value>::Reference BVHTree<Value>::insert(const BoundingBox& bb,Value&& v)
		{
		auto n=boxFindImpl(m_root,bb);
		auto pos=n.push_back(std::move(v));
		return {n,pos};
		}

	template<class Value>
	template<class Visitor>
	void BVHTree<Value>::nodesVisit(const Node& root,Visitor& v) noexcept
		{
		}

	template<class Value>
	template<class Visitor>
	void BVHTree<Value>::nodesVisit(const BoundingBox& bb,Visitor&& v) const noexcept
		{
		const auto& node=boxFindImpl(const_cast<Node&>(m_root),bb);
		nodesVisit(node,std::move(v));
		}


	template<class Value>
	void BVHTree<Value>::remove(Reference& r) noexcept
		{
		r.node.content.erase(r.position);
		}

	template<class Value>
	typename BVHTree<Value>::Node& BVHTree<Value>::boxFindImpl(Node& root,const BoundingBox& bb) noexcept
		{
		for(int k=0;k<8;++k)
			{
			auto child=root.children[k];
			if(child!=nullptr)
				{		
				if(bb.inside(child->bb))
					{return boxFindImpl(child,bb);}
				}
			}
		return root;
		}
	}


#endif