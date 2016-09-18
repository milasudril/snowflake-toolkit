//@ {"targets":[{"name":"triangle.o","type":"object"}]}

#include "triangle.h"
#include "twins.h"

using namespace SnowflakeModel;


//Equation the distance to a plane
static inline float Pi(const Vector& plane_normal,const Point& V_0
	,const Point& P) noexcept
	{return glm::dot(plane_normal,Vector(P-V_0));}

namespace
	{
	template<class T>
	struct Tripple
		{
		T objs[3];
		T& operator[](unsigned int index)
			{return objs[index];}
		const T& operator[](unsigned int index) const
			{return objs[index];}
		};
	}

template<class T>
static inline Tripple<int> i_other(const Tripple<T>& Pi) noexcept
	{
	if(Pi[0]*Pi[1]>0)
		{return Tripple<int>{1,2,0};}
	if(Pi[0]*Pi[2]>0)
		{return Tripple<int>{0,1,2};}
	return Tripple<int>{2,0,1};
	}

template<class T>
static inline Twins<T> sort(const Twins<T>& t) noexcept
	{return t.first<t.second? t : Twins<T>{t.second,t.first};}


//Overlap between two line segments
static inline bool overlap(const Twins<float> a,const Twins<float>& b) noexcept
	{
	if(a.second<=b.first+1e-6f || b.second<=a.first+1e-6f)
		{return 0;}
	return 1;
	}


bool SnowflakeModel::overlap(const Triangle& T_1,const Triangle& T_2) noexcept
	{
	assert(&T_1!=&T_2);
	auto d=glm::cross(T_1.m_normal,T_2.m_normal);

//	T_1
	Tripple<float> pi;
	Tripple<float> t;
	for(unsigned int k=0;k<3;++k)
		{
		pi[k]=Pi(T_2.m_normal,T_2.vertexGet(0),T_1.vertexGet(k));
	//	Robustness check
		pi[k]=fabs(pi[k])>1.e-6f? pi[k]: 0.0f;
		}

	//	Triangles are coplanar or does not intersect
	if(pi[0]*pi[1]>=0.0f && pi[0]*pi[2]>=0.0f)
		{return 0;}

	auto i_ref=i_other(pi);
	for(unsigned int k=0;k<3;++k)
		{t[k]=glm::dot(d,Vector( T_1.vertexGet(i_ref[k])) );}

	auto t_1=sort(Twins<float>
		{
		 (t[0]*pi[i_ref[1]] - t[1]*pi[i_ref[0]])/(pi[i_ref[1]] - pi[i_ref[0]])
		,(t[1]*pi[i_ref[2]] - t[2]*pi[i_ref[1]])/(pi[i_ref[2]] - pi[i_ref[1]])
		});

//	T_2
	for(unsigned int k=0;k<3;++k)
		{
		pi[k]=Pi(T_1.m_normal,T_1.vertexGet(0),T_2.vertexGet(k));
	//	Robustness check
		pi[k]=fabs(pi[k])>1.e-6f? pi[k]: 0.0f;
		}

	//	Triangles are coplanar or does not intersect
	if(pi[0]*pi[1]>=0.0f && pi[0]*pi[2]>=0.0f)
		{return 0;}

	i_ref=i_other(pi);
	for(unsigned int k=0;k<3;++k)
		{t[k]=glm::dot(d,Vector( T_2.vertexGet(i_ref[k]) ) );}

	auto t_2=sort(Twins<float>
		{
		 (t[0]*pi[i_ref[1]] - t[1]*pi[i_ref[0]])/(pi[i_ref[1]] - pi[i_ref[0]])
		,(t[1]*pi[i_ref[2]] - t[2]*pi[i_ref[1]])/(pi[i_ref[2]] - pi[i_ref[1]])
		});

	auto ret=::overlap(t_1,t_2);

	return ret;
	}