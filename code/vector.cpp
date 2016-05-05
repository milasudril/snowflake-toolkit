#ifdef __WAND__
target[name[vector.o] type[object]]
#endif

#include "vector.h"

using namespace SnowflakeModel;

std::pair<Matrix,bool>
SnowflakeModel::vectorsAlign(const Vector& dir,const Vector& dir_target)
	{
//	http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
	auto nu=glm::cross(dir,dir_target);
	auto s=glm::dot(nu,nu);
	auto c=glm::dot(dir,dir_target);
	if(glm::abs(s) < 1e-6)
		{
		SnowflakeModel::Matrix R;
		R= c<0? -R : R;
		R[3][3]=1;
		return {std::move(R),c<0};
		}
	SnowflakeModel::Matrix nux;
	nux[0][0]=0;
	nux[0][1]=nu[2];
	nux[0][2]=-nu[1];
	nux[1][0]=-nu[2];
	nux[1][1]=0;
	nux[1][2]=nu[0];
	nux[2][0]=nu[1];
	nux[2][1]=-nu[0];
	nux[2][2]=0;
	nux[3][3]=0;
	SnowflakeModel::Matrix R;
	R+=nux + nux*nux*(1-c)/s;
	return {R,0};
	}

static inline glm::mat3 orthobasis(const Vector& dir_target)
	{
	auto a=Vector(dir_target[2],dir_target[0],dir_target[1]);
	auto v=glm::normalize(cross(dir_target,a));
	auto w=cross(dir_target,v);

	return glm::mat3(dir_target,v,w);
	}

Matrix SnowflakeModel::vectorsAlign2(const Vector& dir,const Vector& dir_target)
	{
	auto R_2=orthobasis(dir_target);
	auto R_1=orthobasis(dir);

	return Matrix(R_2*glm::transpose(R_1));
	}
